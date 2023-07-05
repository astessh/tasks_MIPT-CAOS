#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int server;

const char CONTENT_LENGTH_STRING[] = "Content-Length: ";
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

void send_request(char* host, char* path) {
  char query[BUFSIZ];
  snprintf(query, BUFSIZ, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, host);
  write(server, query, strlen(query));
}

void disconnect() {
  shutdown(server, 0);
  close(server);
}

void process_response() {
  size_t content_length = 0;
  char response[BUFSIZ];

  ssize_t recv_size = recv(server, response, BUFSIZ, 0);
  char* data_ptr =
      strstr(response, CONTENT_LENGTH_STRING) + strlen(CONTENT_LENGTH_STRING);
  sscanf(data_ptr, "%zu", &content_length);

  data_ptr = strstr(response, CONTENT_DELIMITER) + strlen(CONTENT_DELIMITER);
  ssize_t write_size = write(1, data_ptr, recv_size - (data_ptr - response));
  content_length -= write_size;

  while (content_length > 0) {
    recv_size = recv(server, response, BUFSIZ, 0);
    write_size = write(STDOUT_FILENO, response, recv_size);
    content_length -= write_size;
  }
}

int main(int argc, char* argv[]) {
  connect_to(argv[1]);
  send_request(argv[1], argv[2]);
  process_response();
  disconnect();
  return 0;
}
