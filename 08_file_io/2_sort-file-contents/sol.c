#define _LARGEFILE64_SOURCE

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int comp(const void* a, const void* b) {
  int ra = *(const int*) a;
  int rb = *(const int*) b;
  if (ra < rb) return -1;
  if (ra > rb) return 1;
  return 0;
}

void Sort(int file, int res, int64_t start, int64_t end) {
  const int64_t int_size = sizeof(int);
  const int max_file = 4;
  int64_t size = end - start;
  if (size < max_file) {
    int* array = (int*) malloc(size * int_size);
    lseek64(file, start * int_size, SEEK_SET);
    read(file, array, size * int_size);
    qsort(array, size, sizeof(int), comp);
    lseek64(file, start * int_size, SEEK_SET);
    write(file, array, size * int_size);
    free(array);
  } else {
    Sort(file, res, start, (start + end) / 2);
    Sort(file, res, (start + end) / 2, end);
    lseek64(file, start * int_size, SEEK_SET);
    int64_t fp = start;
    int64_t sp = (start + end) / 2;
    int64_t rp = 0;
    int f, s;
    lseek64(res, 0, SEEK_SET);
    lseek64(file, fp * int_size, SEEK_SET);
    read(file, &f, int_size);
    lseek64(file, sp * int_size, SEEK_SET);
    read(file, &s, int_size);
    while (sp < end || fp < (start + end) / 2) {
      if ((sp < end && fp < (start + end) / 2 && f <= s)
          || (sp >= end && fp < (start + end) / 2)) {
        lseek64(res, rp * int_size, SEEK_SET);
        write(res, &f, int_size);
        fp++;
        lseek64(file, fp * int_size, SEEK_SET);
        read(file, &f, int_size);
      } else if ((sp < end && fp < (start + end) / 2 && s < f)
          || (fp >= (start + end) / 2 && sp < end)) {
        lseek64(res, rp * int_size, SEEK_SET);
        write(res, &s, int_size);
        sp++;
        lseek64(file, sp * int_size, SEEK_SET);
        read(file, &s, int_size);
      }
      rp++;
    }
    lseek64(res, 0, SEEK_SET);
    lseek64(file, start * int_size, SEEK_SET);
    for (int i = 0; i < size; i++) {
      int buf;
      read(res, &buf, int_size);
      write(file, &buf, int_size);
    }
  }
}

int main(int args, char* argv[]) {
  int file = open(argv[1], O_RDWR);
  int temp = open("temp", O_RDWR | O_CREAT);
  if (file < 0) {
    return 1;
  }
  uint64_t size = lseek64(file, 0, SEEK_END) / sizeof(int32_t);
  Sort(file, temp, 0, (int64_t) size);
  lseek(file, 0, SEEK_SET);
  close(file);
  close(temp);
  remove("temp");
  return 0;
}

