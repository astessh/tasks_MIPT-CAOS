/*Программе в аргументе командной строки передается имя файла с бинарными данными в Little-Endian.

Файл хранит внутри себя односвязный список элементов:

struct Item {
  int value;
  uint32_t next_pointer;
};

Поле value храние значение элемента списка, поле next_pointer - позицию в файле (в байтах), указывающую на следующий элемент. Признаком последнего элемента является значение next_pointer, равное 0.

Расположение первого элемента списка (если он существует) - строго в нулевой позиции в файле, расположение остальных - случайным образом.

Выведите на экран через пробел значения элементов в списке в текстовом представлении.

Для работы с файлом использовать только низкоуровневый ввод-вывод POSIX.*/

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
