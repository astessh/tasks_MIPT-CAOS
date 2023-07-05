#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

int server;

const char CONTENT_DELIMITER[] = "\r\n\r\n";

void connect_to(char* host) {
  struct addrinfo connect_info = {
      .ai_family = AF_INET,
      .ai_socktype = SOCK_STREAM
  };
  struct addrinfo* addr = NULL;
  getaddrinfo(host, "http", &connect_info, &addr);

  server = socket(connect_info.ai_family, connect_info.ai_socktype, 0);
  connect(server, addr->ai_addr, addr->ai_addrlen);
}

void send_request(char* host, char* path, char* file_path) {
  char query[BUFSIZ];
  char query_body_buf[BUFSIZ];
  memset(query_body_buf, 0, BUFSIZ);
  memset(query, 0, BUFSIZ);
  struct stat file_st;
  lstat(file_path, &file_st);
  size_t file_size = file_st.st_size;
  snprintf(query, BUFSIZ,
           "POST %s HTTP/1.1\r\nHost: %s\r\nContent-Length: %lu\r\n\r\n",
           path, host, file_size);
  size_t header_len = strlen(query);
  size_t body_len = 0;
  size_t size = 0;
  int file = open(file_path, O_RDONLY);
  while ((size = read(file, query_body_buf, sizeof(query_body_buf))) != 0) {
    strcat(query, query_body_buf);
    body_len += size;
    memset(query_body_buf, 0, BUFSIZ);
  }
  strcat(query, "\r\n\r\n");
  body_len += strlen("\r\n\r\n");
  write(server, query, header_len + body_len);
}

void disconnect() {
  shutdown(server, 0);
  close(server);
}

void Alarm() {
  disconnect();
  exit(0);
}

void process_response() {
  char response[BUFSIZ];
  memset(response, 0, BUFSIZ);
  size_t received = 0;
  struct sigaction alm;
  memset(&alm, 0, sizeof(alm));
  alm.sa_handler = Alarm;
  alm.sa_flags = SA_RESTART;
  sigaction(SIGALRM, &alm, NULL);
  alarm(1);
  while ((received = read(server, response, BUFSIZ)) > 0) {
    char* data_ptr =
        strstr(response, CONTENT_DELIMITER) + strlen(CONTENT_DELIMITER);
    write(STDOUT_FILENO, data_ptr, received - (data_ptr - response));
    memset(response, 0, BUFSIZ);
  }
}

int main(int argc, char* argv[]) {
  connect_to(argv[1]);
  send_request(argv[1], argv[2], argv[3]);
  process_response();
  disconnect();
  return 0;
}
