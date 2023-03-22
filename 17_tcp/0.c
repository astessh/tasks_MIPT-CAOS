/*
Программе передаются два аргумента: строка с IPv4-адресом в стандартной десятичной записи (четыре числа, разделенные точкой), и номер порта.

Программа должна установить соединение с указанным сервером, после чего читать со стандартного потока ввода целые знаковые числа в текстовом формате, и отправлять их в бинарном виде на сервер. Порядок байт - Little Endian.

В ответ на каждое полученное число, сервер отправляет целое число (в таком же формате), и все эти числа необходимо вывести на стандартный поток вывода в текстовом виде.

Если сервер по своей инициативе закроет соединение, то нужно завершить работу с кодом возврата 0.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdint.h>
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
