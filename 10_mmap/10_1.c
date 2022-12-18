/*
 * Программе в аргументе командной строки передается имя файла с бинарными данными в Little-Endian.

Файл хранит внутри себя односвязный список элементов:

struct Item {
  int value;
  uint32_t next_pointer;
};

Поле value храние значение элемента списка, поле next_pointer - позицию в файле (в байтах), указывающую на следующий элемент. Признаком последнего элемента является значение next_pointer, равное 0.

Расположение первого элемента списка (если он существует) - строго в нулевой позиции в файле, расположение остальных - случайным образом.

Выведите на экран значения элементов в списке в текстовом представлении.

Используйте отображение содержимого файла на память.*/

#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

int Execute(int file) {
  if (file < 0) {
    return 1;
  }
  struct stat st;
  fstat(file, &st);
  uint64_t size = st.st_size;
  if (size <= 0) {
    return 0;
  }
  int32_t* content = mmap(NULL, size, PROT_READ, MAP_PRIVATE, file, 0);
  int32_t* cur = content;
  int value = 0;
  uint32_t next_pointer = 0;
  value = *cur;
  cur += 1;
  next_pointer = *cur;
  while (next_pointer != 0) {
    printf("%d ", value);
    cur = content + (next_pointer / sizeof(int32_t));
    value = *cur;
    cur += 1;
    next_pointer = *cur;
  }
  printf("%d ", value);
  munmap(content, st.st_size);
  return 0;
}

int main(int args, char* argv[]) {
  int input = open(argv[1], O_RDONLY);
  int exit_code = Execute(input);
  close(input);
  return exit_code;
}

