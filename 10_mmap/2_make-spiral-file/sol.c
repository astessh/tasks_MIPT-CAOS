#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>

void Inc(int64_t* to_zero, int64_t* to_min, int64_t* to_inc) {
  *to_zero = 0;
  *to_min = -1;
  (*to_inc)--;
}

void Dec(int64_t* to_zero, int64_t* to_max, int64_t* to_dec) {
  *to_zero = 0;
  *to_max = 1;
  (*to_dec)++;
}

int Execute(int file, int32_t m_s, int32_t width) {
  if (file < 0) {
    return 1;
  }
  int64_t size = m_s * m_s * width + m_s;
  lseek(file, size - 1, SEEK_SET);
  write(file, "k", 1);
  char* fil_con = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, file, 0);
  char* arr = fil_con;
  int64_t i = 0;
  int64_t j = -1;
  int64_t ic = 0;
  int64_t jc = 1;
  int64_t j_min = 0;
  int64_t j_max = m_s - 1;
  int64_t i_min = 0;
  int64_t i_max = m_s - 1;
  for (int num = 1; num <= m_s * m_s; num++) {
    if (i == i_max && ic == 1) {
      Inc(&ic, &jc, &j_max);
    }
    if (i == i_min && ic == -1) {
      Dec(&ic, &jc, &j_min);
    }
    if (j == j_min && jc == -1) {
      Inc(&jc, &ic, &i_max);
    }
    if (j == j_max && jc == 1) {
      Dec(&jc, &ic, &i_min);
    }
    i += ic;
    j += jc;
    char buffer[width];
    sprintf(buffer, "%d", num);
    uint32_t rad_size = strlen(buffer);
    memset(buffer, ' ', sizeof(buffer));
    sprintf(buffer + (width - rad_size), "%d", num);
    int64_t ind = (m_s * width * i) + (j * width) + i;
    memcpy(arr + ind, buffer, sizeof(buffer));
  }
  for (int k = 0; k < m_s; k++) {
    *(arr + (k + 1) * m_s * width + k) = '\n';
  }
  msync(fil_con, size, MS_SYNC);
  munmap(fil_con, size);
  return 0;
}

int main(int args, char* argv[]) {
  int input = open(argv[1], O_RDWR | O_CREAT, 0640);
  int m_s = atoi(argv[2]);
  int width = atoi(argv[3]);
  int exit_code = Execute(input, m_s, width);
  close(input);
  return exit_code;
}
