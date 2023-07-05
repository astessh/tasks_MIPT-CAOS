#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
  pthread_mutex_t* mutex;
  double* numbers;
  int i;
  int N;
  int k;
} data_t;

void* thread_func(data_t* data) {
  pthread_mutex_lock(data->mutex);
  for (int j = 0; j < data->N; ++j) {
    data->numbers[(data->i - 1 + data->k) % data->k] += 0.99;
    data->numbers[data->i] += 1;
    data->numbers[(data->i + 1) % data->k] += 1.01;
  }
  pthread_mutex_unlock(data->mutex);
  return 0;
}

int main(int argc, char* argv[]) {
  int N = atoi(argv[1]);
  int k = atoi(argv[2]);
  double* numbers = (double*) calloc(k, sizeof(double));
  pthread_t* threads = (pthread_t*) calloc(k, sizeof(pthread_t));
  data_t* args = (data_t*) calloc(k, sizeof(data_t));
  pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
  for (int i = 0; i < k; ++i) {
    args[i].mutex = &mu;
    args[i].numbers = numbers;
    args[i].i = i;
    args[i].N = N;
    args[i].k = k;
    pthread_create(&threads[i], NULL, (void* (*)(void*)) thread_func,
                   (void*) &args[i]);
  }
  for (int i = 0; i < k; i++) {
    pthread_join(threads[i], NULL);
  }
  for (int i = 0; i < k; i++) {
    printf("%.10g\n", numbers[i]);
  }
  fflush(stdout);
  free(args);
  free(threads);
  free(numbers);
  //mmmm text
  return 0;
}
