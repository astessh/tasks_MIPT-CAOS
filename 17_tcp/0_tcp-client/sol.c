#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char** argv) {
  uint32_t address = inet_addr(argv[1]);
  uint16_t port = htons(atoi(argv[2]));
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = port;
  addr.sin_addr.s_addr = address;
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    perror("error");
    exit(1);
  }
  int conn = connect(sock, (struct sockaddr*) &addr, sizeof(struct sockaddr_in));
  if (conn == -1) {
    close(sock);
    perror("error");
    exit(1);
  }
  int my, get;
  while (scanf("%d", &my) != EOF) {
    if (write(sock, &my, sizeof(int)) < 1 || read(sock, &get, sizeof(int)) < 1) {
      break;
    }
    printf("%d\n", get);
  }
  close(sock);
  shutdown(sock, SHUT_RDWR);
  return 0;
}
