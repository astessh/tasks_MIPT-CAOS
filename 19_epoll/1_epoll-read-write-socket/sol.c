#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <signal.h>

void Error(const char* message) {
  perror(message);
  exit(1);
}

static void epoll_ctl_add(int epfd, int fd, uint32_t events) {
  struct epoll_event ev;
  ev.events = events;
  ev.data.fd = fd;
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    Error("epoll error");
  }
}

static int setnonblocking(int sockfd) {
  if (fcntl(sockfd, F_SETFD, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1) {
    return -1;
  }
  return 0;
}

void ReCaps(char* buff, size_t size) {
  for (size_t i = 0; i < size; i++) {
    if (buff[i] >= 'a' && buff[i] <= 'z') {
      buff[i] -= 32;
    }
  }
}

void Exit() {
  exit(0);
}

int main(int argc, char* argv[]) {
  const size_t kBufSize = 4096;
  const size_t kPort = atoi(argv[1]);

  struct sigaction term;
  memset(&term, 0, sizeof(term));
  term.sa_handler = Exit;
  term.sa_flags = SA_RESTART;
  sigaction(SIGTERM, &term, NULL);

  char buf[kBufSize];
  struct sockaddr_in srv_addr;
  struct sockaddr_in cli_addr;
  struct epoll_event events[SOMAXCONN];

  int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sock == -1) {
    Error("socket error");
  }

  memset(&srv_addr, 0, sizeof(struct sockaddr_in));
  srv_addr.sin_family = AF_INET;
  srv_addr.sin_addr.s_addr = INADDR_ANY;
  srv_addr.sin_port = htons(kPort);
  int res = bind(listen_sock, (struct sockaddr*) &srv_addr, sizeof(srv_addr));
  if (res == -1) {
    Error("bind error");
  }

  setnonblocking(listen_sock);
  res = listen(listen_sock, SOMAXCONN);
  if (res == -1) {
    Error("listen error");
  }

  int epfd = epoll_create(1);
  epoll_ctl_add(epfd, listen_sock, EPOLLIN | EPOLLOUT | EPOLLET);

  socklen_t socklen = sizeof(cli_addr);
  while (1) {
    int nfds = epoll_wait(epfd, events, SOMAXCONN, -1);
    for (int i = 0; i < nfds; i++) {
      if (events[i].data.fd == listen_sock) {
        int conn_sock =
            accept(listen_sock, (struct sockaddr*) &cli_addr, &socklen);
        cli_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        setnonblocking(conn_sock);
        epoll_ctl_add(epfd,
                      conn_sock,
                      EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP);
      } else if (events[i].events & EPOLLIN) {
        memset(buf, 0, sizeof(buf));
        int n = read(events[i].data.fd, buf, sizeof(buf));
        if (n <= 0) {
          break;
        } else {
          ReCaps(buf, kBufSize);
          write(events[i].data.fd, buf, strlen(buf));
        }
      }
      if (events[i].events & (EPOLLRDHUP | EPOLLHUP)) {
        epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
        close(events[i].data.fd);
      }
    }
  }
}
