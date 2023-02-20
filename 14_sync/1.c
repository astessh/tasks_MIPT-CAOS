/*
Программа принимает три аргумента: два 64-битных числа A и B, и 32-битное число N.

Затем создается дополнительный поток, которые генерирует простые числа в диапазоне от A до B включительно, и сообщает об этом основному потоку, с которого началось выполнение функции main.

Главный поток выводит на стандартный поток вывода каждое полученное число и завершает свою работу после того, как получит N чисел.

Запрещено использовать глобальные переменные.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint-gcc.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct control {
  uint64_t value;
  uint64_t A;
  pthread_mutex_t* mutex;
  pthread_cond_t* cond_var;
  pthread_cond_t* cond_var2;
  uint32_t N;
} control;

bool is_prime(uint64_t x) {
  for (size_t i = 2; i * i <= x; ++i) {
    if (x % i == 0) {
      return false;
    }
  }
  return true;
}
void* producer(void* arg) {
  control* state = (control*) arg;
  pthread_mutex_lock(state->mutex);
  uint64_t current = state->A;
  uint32_t counter = 0;
  while (counter < state->N) {
    while (!is_prime(current)) {
      current += 1;
    }
    state->value = current;
    pthread_cond_signal(state->cond_var);
    pthread_cond_wait(state->cond_var2, state->mutex);
    current++;
    counter++;
  }
  return NULL;
}

void consumer(control* state) {
  uint32_t got_numbers = 0;
  while (got_numbers < state->N) {
    pthread_cond_wait(state->cond_var, state->mutex);
    printf("%lu ", state->value);
    got_numbers++;
    pthread_cond_signal(state->cond_var2);
  }
}

int main(int argc, char* argv[]) {
  uint64_t A = strtoll(argv[1], NULL, 10); // text
  uint64_t B = strtoll(argv[2], NULL, 10);
  uint32_t N = strtol(argv[3], NULL, 10);
  pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;
  pthread_cond_t cond_var2 = PTHREAD_COND_INITIALIZER;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_t thread;
  control state;
  state.mutex = &mutex;
  state.A = A;
  state.N = N;
  state.cond_var = &cond_var;
  state.cond_var2 = &cond_var2;
  pthread_mutex_lock((&mutex));
  pthread_create(&thread, NULL, producer, (void*) &state);
  consumer(&state);
  pthread_mutex_unlock(&mutex);
  pthread_join(thread, NULL);
  return 0;
}
