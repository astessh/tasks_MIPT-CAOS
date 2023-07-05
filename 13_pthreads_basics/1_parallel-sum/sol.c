#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <bits/local_lim.h>

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

