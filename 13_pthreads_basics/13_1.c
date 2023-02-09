/*
Единственным аргументом программы является целое число N>1 - число потоков, которые нужно создать.

На стандартном потоке ввода задается последовательность целых чисел.

Реализуйте программу, которая запускает N потоков, каждый из которых читает числа со стандартного потока ввода, и вычисляет частичные суммы. Учтите, что количество потоков необязательно равно количеству чисел.

На стандартный поток вывода необходимо вывести итоговую сумму всех чисел.

Минимизируйте объем используемой памяти настолько, насколько это возможно. Обратите внимание на ограничение по памяти.
*/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <bits/local_lim.h>
#include <limits.h>

static void* thread_func(void* arg) {
  int64_t cur = 0;
  if (scanf("%ld", &cur) >= 1) {
    *(int64_t*) arg += cur;
  }
  return NULL;
}

int main(int argc, char* argv[]) {
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setguardsize(&attr, 0);
  pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
  pthread_t thread;
  int64_t sum = 0;
  int64_t cur = 0;
  for (int i = 0; i < atoi(argv[1]); i++) {
    pthread_create(&thread, &attr, thread_func, &sum);
    pthread_join(thread, NULL);
  }
  pthread_attr_destroy(&attr);
  while (scanf("%ld", &cur) >= 1) {
    sum += cur;
  }
  printf("%ld", sum);
  return 0;
}

