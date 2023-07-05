#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

int Execute(int file) {
  if (file < 0) {
    return 1;
  }
  int value = 0;
  uint32_t next_pointer = 0;
  if (read(file, &value, sizeof(value)) <= 0) {
    return 0;
  }
  read(file, &next_pointer, sizeof(next_pointer));
  while (next_pointer != 0) {
    printf("%d ", value);
    lseek(file, next_pointer, SEEK_SET);
    read(file, &value, sizeof(value));
    read(file, &next_pointer, sizeof(next_pointer));
  }
  printf("%d ", value);
  return 0;
}

int main(int args, char* argv[]) {
  int input = open(argv[1], O_RDONLY);
  int exit_code = Execute(input);
  close(input);
  return exit_code;
}
