/*
Аргументом программы является целое число - номер порта на сервере localhost.

Программа читает со стандартного потока ввода целые числа в тектовом формате, и отправляет их в бинарном виде (little-endian) на сервер как UDP-сообщение.

В ответ сервер отправляет целое число (также в бинарном виде, little-endian), которое необходимо вывести на стандартный поток вывода.

*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  int32_t port_number = strtol(argv[1], NULL, 10);
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (-1 == sock) {
    perror("socket");
    exit(1);
  }
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_addr = inet_addr("127.0.0.1"),
      .sin_port = htons(port_number)
  };
  int send_num, recv_num;
  while (scanf("%d", &send_num) > 0) {
    sendto(sock, &send_num, sizeof(send_num), 0, (const struct sockaddr*) &addr,
           sizeof(addr));
    recvfrom(sock, &recv_num, sizeof(recv_num), 0, NULL, NULL);
    printf("%d\n", recv_num);
  }
  close(sock);
}
