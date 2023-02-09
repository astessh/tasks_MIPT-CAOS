/*
На стандартном потоке ввода задается последовательность целых чисел.

Необходимо прочитать эти числа, и вывести их в обратном порядке.

Каждый поток может прочитать или вывести только одно число.

Используйте многопоточность, запуск процессов запрещен.
*/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static void* thread_func(void* arg) {
  int val;
  if (scanf("%d ", &val) != 1) {
    return NULL;
  }
  pthread_t next_thread;
  pthread_create(&next_thread, NULL, thread_func, 0);
  pthread_join(next_thread, NULL);
  printf("%d ", val);
  return NULL;
}

int main() {
  pthread_t thread;
  pthread_create(&thread, NULL, thread_func, 0);
  pthread_join(thread, NULL);
  return 0;
}

