/*
Реализуйте Lock-Free односвязный список из элементов:

typedef struct Item {
  struct Item *next;
  int64_t      value;
} item_t;

Программа принимает в качестве аргумента значения N - количество потоков, и k - количество элементов, создаваемых каждым потоком.

Этих потоков может быть очень много - несколько десятков!

Каждый поток должен добавить в односвязный список k элементов со значениями от i*k до (i+1)*k, где i - это номер потока от 0 до N.

После завершения работы всех потоков выведите все значения полученного односвязного списка.

Запрещено использовать мьютексы, семафоры или активное ожидание.

Минимизируйте используемый объем памяти.

Для уменьшения времени простоя используйте sched_yield в конце каждой итерации.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <stdatomic.h>

typedef struct Item {
  struct Item* _Atomic next;
  int64_t value;
} item_t;

typedef struct control {
  item_t* head;
  uint32_t i;
  uint32_t k;
} control;

void* inserter(void* arg) {
  control* _Atomic state = (control*) arg;
  for (uint64_t i = state->i * state->k; i < state->k * (state->i + 1); i++) {
    item_t* _Atomic new = (item_t*) calloc(1, sizeof(item_t));
    new->value = i;
    new->next = atomic_exchange(&state->head->next, new);
  }
  return NULL;
}

int main(int argc, char* argv[]) {
  uint64_t N = strtoll(argv[1], NULL, 10);
  uint64_t K = strtoll(argv[2], NULL, 10);
  pthread_t* thread = (pthread_t*) calloc(N, sizeof(pthread_t));
  control* controls = (control*) calloc(N, sizeof(control));
  item_t* head = (item_t*) calloc(1, sizeof(item_t));
  item_t* end = (item_t*) calloc(1, sizeof(item_t));
  head->next = end;
  end->next = (struct Item*) 0;
  for (uint64_t i = 0; i < N; i++) {
    controls[i].head = head;
    controls[i].i = i;
    controls[i].k = K;
    pthread_create(&thread[i], NULL, inserter, (void*) &controls[i]);
  }
  for (uint64_t i = 0; i < N; i++) {
    pthread_join(thread[i], NULL);
  }
  item_t* prev = head;
  for (item_t* pointer = head->next; pointer != end; pointer = pointer->next) {
    free(prev);
    printf("%lu ", pointer->value);
    prev = pointer;
  }
  free(prev);
  free(end);
  free(thread);
  free(controls);
  return 0;
}
