#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

int main() {
  char bytes[2048];
  memset(bytes, 0, sizeof(bytes));
  for (size_t i = 0; scanf("%c", &bytes[i]) != EOF; ++i);
  unsigned char hash_arr[SHA512_DIGEST_LENGTH];
  SHA512(bytes, strlen(bytes), hash_arr);
  printf("0x");
  for (int j = 0; j < SHA512_DIGEST_LENGTH; ++j) {
    printf("%02x", hash_arr[j]);
  }
  return 0;
}


