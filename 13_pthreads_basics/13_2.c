/*
Программе передается единственный аргумент - начальное значение 0<N≤100.

Необходимо создать два потока:

    Принимает число, уменьшает его значение на 3, выводит его на стандартный поток вывода, и передает другому потоку.
    Принимает число, увеличивает его значение на 5, выводит его на стандартный поток вывода, и передает другому потоку.

Начально значение передается первому потоку.

Взаимодействие прекращается, когда значение будет равно 0, либо станет строго больше 100. Последний поток, который получит число 0 или число >100, должен вывести его на стандартный поток вывода, и завершить работу.

Для передачи чисел между потоками используйте пару сокетов (man socketpair).
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct state {
  int starting_num;
  int sockets[2];
  bool started;
  bool finished;
} state;

static void* first_fun(void* arg) {
  int N;
  state* st = (state*) arg;
  while (!st->finished) {
    if (!st->started) {
      st->started = true;
      N = st->starting_num;
    } else {
      read(st->sockets[0], &N, sizeof(int));
      if (st->finished) {
        break;
      }
    }
    N -= 3;
    if (N == 0 || N > 100) {
      st->finished = true;
    }
    printf("%d\n", N);
    write(st->sockets[0], &N, sizeof(int));
  }
  return NULL;
}

static void* second_fun(void* arg) {
  int N;
  state* st = (state*) arg;
  while (!st->finished) {
    read(st->sockets[1], &N, sizeof(int));
    if (st->finished) {
      break;
    }
    N += 5;
    if (N == 0 || N > 100) {
      st->finished = true;
    }
    printf("%d\n", N);
    write(st->sockets[1], &N, sizeof(int));
  }
  return NULL;
}

int main(int argc, char* argv[]) {
  state st;
  st.started = false;
  st.finished = false;
  st.starting_num = atoi(argv[1]);
  socketpair(AF_UNIX, SOCK_STREAM, 0, st.sockets);
  pthread_t first, second;
  pthread_create(&first, NULL, first_fun, (void*) &st);
  pthread_create(&second, NULL, second_fun, (void*) &st);
  pthread_join(first, NULL);
  pthread_join(second, NULL);
  return 0;
}
