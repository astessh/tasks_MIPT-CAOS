#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

size_t package_counter = 0;

typedef struct icmp_header {
  uint8_t icmp_type;
  uint8_t icmp_code;
  uint16_t check_sum;
  uint16_t icmp_id;
  uint16_t icmp_seq;
} icmp_header_t;

void Error(const char* message) {
  perror(message);
  exit(1);
}

void Alarm() {
  printf("%zu\n", package_counter);
  exit(0);
}

void write_package(char* package, size_t size) {
  icmp_header_t* header = (icmp_header_t*) package;
  memset(package, 0, size);
  header->icmp_type = 8;
  const char text[] = "astessh";
  for (char i = 0; i < (48 - (char) sizeof(icmp_header_t)); ++i) {
    package[i + sizeof(icmp_header_t)] = text[i % sizeof(text)];
  }
}

uint16_t checksum(void* package, size_t size) {
  uint16_t* buf = package;
  uint32_t sum = 0;
  for (sum = 0; size > 1; size -= 2) {
    sum += *buf++;
  }
  if (size == 1) {
    sum += *buf;
  }
  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  uint16_t result = ~sum;
  return result;
}

int CheckData(const char* reply, const char* req, size_t size) {
  for (size_t i = sizeof(icmp_header_t); i < size; i++) {
    if (reply[i] != req[i]) {
      return 0;
    }
  }
  return 1;
}

int CheckPackage(void* reply, void* req, size_t size) {
  icmp_header_t* header = (icmp_header_t*) reply;
  uint16_t got_checksum = header->check_sum;
  header->check_sum = 0;
  if (checksum(reply, size) == got_checksum && CheckData(reply, req, size)) {
    return 1;
  }
  return 0;
}

int main(int argc, char* argv[]) {
  uint32_t address = inet_addr(argv[1]);
  size_t interval = strtoll(argv[3], NULL, 10);

  struct sigaction alm;
  memset(&alm, 0, sizeof(alm));
  alm.sa_handler = Alarm;
  alm.sa_flags = SA_RESTART;
  sigaction(SIGALRM, &alm, NULL);
  alarm(strtoll(argv[2], NULL, 10));

  int socket_d = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
  if (socket_d == -1) {
    close(socket_d);
    Error("socket creation error");
  }

  struct sockaddr_in adr;
  memset(&adr, 0, sizeof(adr));
  adr.sin_family = AF_INET;
  adr.sin_addr.s_addr = address;
  if (connect(socket_d, (struct sockaddr*) &adr, sizeof(adr)) == -1) {
    close(socket_d);
    Error("connection error");
  }

  const uint32_t size = 48;
  char package_sent[size];
  char package_received[size];
  write_package(package_sent, size);
  while (1) {
    ssize_t
        status = sendto(socket_d, package_sent, size, 0, (struct sockaddr*) &adr,
                        sizeof(adr));
    if (status == -1) {
      Error("Sending error");
    }
    int IsCorrect = 0;
    do {
      status = recvfrom(socket_d, package_received, size, 0, NULL, NULL);
      IsCorrect = CheckPackage(package_received, package_sent, size);
    } while (!IsCorrect);
    ++package_counter;
    usleep(interval);
  }
  return 0;
}

