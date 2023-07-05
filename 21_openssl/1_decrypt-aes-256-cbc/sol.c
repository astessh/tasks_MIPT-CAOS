#include <string.h>
#include <unistd.h>
#include <openssl/evp.h>

int main(int argc, char* argv[]) {
  const int kSize = 1024 * 1024;
  char salt[8];
  read(STDIN_FILENO, salt, 8);
  read(STDIN_FILENO, salt, 8);

  EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
  EVP_CIPHER_CTX_init(ctx);
  unsigned char key[EVP_CIPHER_key_length(EVP_aes_256_cbc())];
  unsigned char iv[EVP_CIPHER_iv_length(EVP_aes_256_cbc())];

  EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), salt, argv[1],
                 strlen(argv[1]), 1, key, iv);
  EVP_DecryptInit(ctx, EVP_aes_256_cbc(), key, iv);

  int read_bytes = 0;
  int n = 0;
  unsigned char bytes[kSize];
  unsigned char output[kSize];
  while ((n = read(0, bytes + read_bytes, sizeof(bytes) - read_bytes - 1)) > 0) {
    read_bytes += n;
  }
  EVP_DecryptUpdate(ctx, output, &read_bytes, bytes, read_bytes);
  int out = 0;
  EVP_DecryptFinal_ex(ctx, output + read_bytes, &out);
  read_bytes += out;
  for (int i = 0; i < read_bytes; ++i) {
    printf("%c", output[i]);
  }
  EVP_CIPHER_CTX_cleanup(ctx);
  return 0;
}
