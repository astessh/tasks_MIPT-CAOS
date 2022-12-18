/*Реализуйте на языке Си программу, которая копирует содержимое потока ввода на поток вывода.

Использование стандартной библиотеки Си запрещено, единственная доступная функция - это syscall(2).

Точка входа в программу - функция _start.

Для использования syscall можно включить в текст программы следующее объявление:*/

#include <sys/syscall.h>

extern long syscall(long number, ...);

int _start() {
  char sym;
  while (syscall(SYS_read, 0, &sym, 1) != 0) {
    syscall(SYS_write, 1, &sym, 1);
  }
  syscall(SYS_exit, 0);
}
