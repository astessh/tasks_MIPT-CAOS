/*
Реализуйте функцию с сигнатурой:

          extern size_t
          read_data_and_count(size_t N, int in[N])
        

которая читает данные из файловых дескрипторов in[X] для всех 0 ≤ X < N , и возвращает суммарное количество прочитанных байт из всех файловых дескрипторов.

Скорость операций ввода-вывода у файловых дескрипторов - случайная. Необходимо минимизировать суммарное астрономическое время чтения данных.

По окончании чтения необходимо закрыть все файловые дескрипторы.

Указание: используйте неблокирующий ввод-вывод. Для тестирования можно использовать socketpair.
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/epoll.h>

typedef struct {
  int fd;
  size_t count;
  bool done;
} data_t;

static data_t* create_event_data(int epoll_fd, int fd) {
  int flags = fcntl(fd, F_GETFL);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  data_t* data = calloc(1, sizeof(*data));
  data->fd = fd;

  struct epoll_event event_ready_read;
  event_ready_read.events = EPOLLIN;
  event_ready_read.data.ptr = data;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event_ready_read);

  return data;
}

static void process_pair_ready_read(data_t* data) {
  char buffer[4096];
  ssize_t cnt = read(data->fd, buffer, sizeof(buffer));
  if (cnt > 0) {
    data->count += cnt;
  } else if (0 == cnt) {
    data->done = true;
    close(data->fd);
  }
}

static void process_epoll_event(struct epoll_event* event, size_t* files_left) {
  const uint32_t mask = event->events;
  data_t* data = event->data.ptr;

  if (mask & EPOLLIN) {
    process_pair_ready_read(data);
  }

  if (data->done) {
    *files_left -= 1;
  }
}

size_t read_data_and_count(size_t N, int in[N]) {
  const size_t kMaxPendingEvents = 10000;
  data_t** entries = calloc(N, sizeof(*entries));
  int epoll_fd = epoll_create1(0);
  for (size_t i = 0; i < N; ++i) {
    entries[i] = create_event_data(epoll_fd, in[i]);
  }
  size_t files_left = N;
  struct epoll_event pending[kMaxPendingEvents];
  while (files_left > 0) {
    int n = epoll_wait(epoll_fd, pending, kMaxPendingEvents, -1);
    for (int i = 0; i < n; ++i) {
      process_epoll_event(&pending[i], &files_left);
    }
  }
  close(epoll_fd);
  size_t result = 0;
  for (size_t i = 0; i < N; ++i) {
    result += entries[i]->count;
  }
  free(entries);
  return result;
} // asd
