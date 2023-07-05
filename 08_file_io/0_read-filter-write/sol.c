#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int Execute(int in, int out_digits, int out_other) {
  int exit_code = 0;
  if (-1 == in) {
    exit_code = 1;
    perror("Open input file");
    return exit_code;
  }
  if (-1 == out_digits || -1 == out_other) {
    exit_code = 2;
    return exit_code;
  }
  ssize_t in_read;
  char in_byte;
  int out;
  while ((in_read = read(in, &in_byte, sizeof(in_byte))) > 0) {
    out = '0' <= in_byte && in_byte <= '9' ? out_digits : out_other;
    if (-1 == write(out, &in_byte, sizeof(in_byte))) {
      exit_code = 3;
      return exit_code;
    }
  }
  if (-1 == in_read) {
    exit_code = 3;
    return exit_code;
  }
  return exit_code;
}

int main(int argc, char* argv[]) {
  int exit_code = 0;
  int in = open(argv[1], O_RDONLY);
  int out_digits = open(argv[2], O_WRONLY | O_CREAT, 0640);
  int out_other = open(argv[3], O_WRONLY | O_CREAT, 0640);
  exit_code = Execute(in, out_digits, out_other);
  close(in);
  close(out_digits);
  close(out_other);
  return exit_code;
}
