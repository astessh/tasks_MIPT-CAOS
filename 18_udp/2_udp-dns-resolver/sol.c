#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

typedef struct DNS_header {
  uint16_t transaction_id;
  uint16_t flags;
  uint16_t questions;
  uint16_t answer_rrs;
  uint16_t authority_rrs;
  uint16_t additional_rrs;
} DNS_header;

typedef struct Query {
  uint16_t type;
  uint16_t class;
} Query;

void Error(const char* message) {
  perror(message);
  exit(1);
}

void ChangeToDnsNameFormat(char* dns, char* host) {
  int lock = 0, i;
  strcat((char*) host, ".");

  for (i = 0; i < strlen((char*) host); i++) {
    if (host[i] == '.') {
      *dns++ = i - lock;
      for (; lock < i; lock++) {
        *dns++ = host[lock];
      }
      lock++;
    }
  }
  *dns++ = '\0';
}

size_t write_package(char* host, char* package, size_t size) {
  DNS_header* header = (DNS_header*) package;
  memset(package, 0, size);
  header->transaction_id = getpid();
  header->flags = 0x0001;
  header->questions = htons(1);
  char* name = (char*) &package[sizeof(DNS_header)];
  ChangeToDnsNameFormat(name, host);
  Query* info = (struct Query*) &package[sizeof(DNS_header)
      + (strlen((const char*) name) + 1)];
  info->type = htons(1);
  info->class = htons(1);
  return sizeof(DNS_header) + strlen((const char*) name) + 1 + sizeof(Query);
}

uint32_t GetIp(char package[], size_t size_sent) {
  DNS_header* header = (DNS_header*) package;
  uint16_t size = ntohs(header->answer_rrs);
  char* pointer = package + size_sent;
  for (uint16_t i = 0; i < size; i++) {
    uint16_t i_size = ntohs(*(uint16_t * )(pointer + 10));
    pointer += 12 + i_size;
  }
  return *(uint32_t * )(pointer - 4);
}

int main(int argc, char* argv[]) {
  const uint32_t kDns = inet_addr("8.8.8.8");
  const uint32_t kMaxSize = 512;

  int socket_d = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (socket_d == -1) {
    close(socket_d);
    Error("socket creation error");
  }

  struct sockaddr_in adr;
  memset(&adr, 0, sizeof(adr));
  adr.sin_family = AF_INET;
  adr.sin_addr.s_addr = kDns;
  adr.sin_port = htons(53);
  char package_sent[kMaxSize];
  char package_received[kMaxSize];
  char* host_raw[kMaxSize];
  memset(host_raw, 0, kMaxSize);

  while (fgets((char*) host_raw, (int) kMaxSize, stdin) != NULL) {
    memset(package_sent, 0, kMaxSize);
    memset(package_received, 0, kMaxSize);
    strtok((char*) host_raw, "\n");
    if (strlen((const char*) host_raw) < 4) {
      break;
    }

    size_t size_sent = write_package((char*) host_raw, package_sent, kMaxSize);
    ssize_t status = sendto(socket_d, package_sent, size_sent, 0,
                            (struct sockaddr*) &adr, sizeof(adr));
    if (status == -1) {
      Error("Sending error");
    }

    size_t size_rec = 0;
    status = recvfrom(socket_d, package_received, kMaxSize, 0,
                      (struct sockaddr*) &adr, (socklen_t * ) & size_rec);
    if (status == -1) {
      Error("Receiving error");
    }

    char str_ip[20];
    uint32_t ip = GetIp(package_received, size_sent);
    inet_ntop(AF_INET, &ip, str_ip, INET_ADDRSTRLEN);
    printf("%s\n", str_ip);
    memset(host_raw, 0, kMaxSize);
  }
  return 0;
}


