/*Реализуйте на языке Си программу, которая выводит "Hello, World!".

Использование стандартной библиотеки Си запрещено, единственная доступная функция - это syscall(2).

Точка входа в программу - функция _start.

Для использования syscall можно включить в текст программы следующее объявление:

long syscall(long number, ...);*/

#include <sys/syscall.h>
long syscall(long number, ...);
int _start () {
	char* word = "Hello, World!\n";
	syscall(SYS_write,1, word, 14);
	syscall(SYS_exit, 0);
}

