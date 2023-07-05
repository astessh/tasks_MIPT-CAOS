#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint-gcc.h>

int main(int argc, char* argv[]) {
  int filename = open(argv[1], O_RDONLY);
  struct stat st;
  fstat(filename, &st);
  uint64_t size = st.st_size;
  char* contents = mmap(NULL, size, PROT_READ, MAP_PRIVATE, filename, 0);
  char* entry = contents;
  while ((entry = memmem(entry, size, argv[2], strlen(argv[2]))) != NULL) {
    uint64_t diff = entry - contents;
    size = st.st_size - diff - 1;
    printf("%lu ", diff);
    ++entry;
  }
  munmap(contents, st.st_size);
  close(filename);
  return 0;
}
