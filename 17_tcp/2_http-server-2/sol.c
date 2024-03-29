#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

void Error(const char* str) {
  for (int i = 3; i < 1024; i++) {
    shutdown(i, SHUT_RDWR);
    close(i);
  }
  perror(str);
  exit(1);
}

void Shutdown() {
  for (int i = 3; i < 1024; i++) {
    shutdown(i, SHUT_RDWR);
    close(i);
  }
  exit(0);
}

void ans200(int client_socket, const char* file_path) {
  if (access(file_path, X_OK) == -1) {
    struct stat file_st;
    lstat(file_path, &file_st);
    int file_size = file_st.st_size;
    dprintf(client_socket, "HTTP/1.1 200 OK\r\n");
    dprintf(client_socket, "Content-Length: %d\r\n\r\n", file_size);
    int file = open(file_path, O_RDONLY);
    char buff[4096];
    memset(buff, 0, 4096);
    size_t size = 0;
    while ((size = read(file, buff, sizeof(buff))) != 0) {
      write(client_socket, buff, size);
    }
    close(file);
    dprintf(client_socket, "\r\n");
  } else {
    pid_t pid = fork();
    if (pid == 0) {
      dprintf(client_socket, "HTTP/1.1 200 OK\r\n");
      dup2(client_socket, 1);
      close(client_socket);
      execlp(file_path, file_path, NULL);
    } else {
      waitpid(pid, 0, 0);
    }
  }
}

void ans403(int client_socket) {
  dprintf(client_socket, "HTTP/1.1 403 Forbidden\r\n");
  dprintf(client_socket, "Content-Length: 0\r\n\r\n");
}

void ans404(int client_socket) {
  dprintf(client_socket, "HTTP/1.1 404 Not Found\r\n");
  dprintf(client_socket, "Content-Length: 0\r\n\r\n");
}

void answer(int client_socket, const char* path, char* request) {
  const char* file = strtok(request + strlen("GET "), " ");
  char file_path[4096];
  memset(file_path, 0, 4096);
  sprintf(file_path, "%s/%s", path, file);
  if (access(file_path, F_OK) == -1) {
    ans404(client_socket);
  } else if (access(file_path, R_OK) == -1) {
    ans403(client_socket);
  } else {
    ans200(client_socket, file_path);
  }
}

int main(int argc, char** argv) {
  struct sigaction term;
  memset(&term, 0, sizeof(term));
  term.sa_handler = Shutdown;
  term.sa_flags = SA_SIGINFO;
  sigaction(SIGTERM, &term, NULL);
  sigaction(SIGINT, &term, NULL);

  uint32_t port = htons(atoi(argv[1]));
  char* path = argv[2];
  int socket_d = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_d == -1) {
    close(socket_d);
    Error("socket creation error");
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = port;
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  if (bind(socket_d, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) ==
      -1) {
    Error("bind error");
  }

  if (listen(socket_d, SOMAXCONN) == -1) {
    Error("lister error");
  }

  int client_socket;
  const size_t buf_size = 8192;
  char request[buf_size];

  while (1) {
    client_socket = accept(socket_d, NULL, NULL);
    if (client_socket == -1) {
      Error("accept error");
    }
    memset(request, 0, buf_size);
    size_t len_read = 0;
    while (1) {
      char buff[4096];
      memset(buff, 0, sizeof(buff));
      read(client_socket, buff, sizeof(buff));
      strcpy(request + len_read, buff);
      len_read += strlen(buff);
      if (strcmp(request + strlen(request) - 4, "\r\n\r\n") == 0) {
        answer(client_socket, path, request);
        break;
      }
    }
    close(client_socket);
  }
  Shutdown();
}

