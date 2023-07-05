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
