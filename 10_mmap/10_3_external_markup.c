/*
 * Необходимо реализовать функциональность функций, аналогичных malloc и free, которые выделяют память в явно заданном swap-файле (файле подкачки), который существует на диске, и имеет некоторый фиксированный размер.

Реализуйте функции:

    extern void*
    my_malloc(size_t size);

    extern void
    my_free(void *ptr);

которые ведут аналогичным образом с malloc и free.

А также функции, которые вызываются один раз: в начале работы программы, и в конце:

    extern void
    myalloc_initialize(int fd);

    extern void
    myalloc_finalize();

Используйте стратегию наиболее экономичного расхода памяти в файле подкачки. */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

typedef struct MemBlock {
  size_t start;
  size_t size;
  size_t prev_size;
  int free;
} MemBlock;

typedef struct Bucket {
  size_t len;
  MemBlock* arr;
} Bucket;

int InBucket(Bucket* bucket, size_t val) {
  for (int i = 0; i < bucket->len; i++) {
    if (bucket->arr[i].start == val) {
      return i;
    }
  }
  return -1;
}

typedef struct HashTable {
  Bucket* table_;
  size_t width_;
} HashTable;

HashTable* Create(size_t width) {
  HashTable* new = (HashTable*) malloc(sizeof(HashTable));
  new->table_ = (Bucket*) malloc(sizeof(Bucket) * width);
  for (int i = 0; i < width; i++) {
    new->table_[i].len = 0;
    new->table_[i].arr = (MemBlock*) malloc(0);
  }
  new->width_ = width;
  return new;
}

void Delete(HashTable* self) {
  for (int i = 0; i < self->width_; i++) {
    free(self->table_[i].arr);
  }
  free(self->table_);
  free(self);
}

uint64_t GetHash(size_t val, size_t width) {
  return ((val * 971) / 153) % width;
}

void Add(HashTable* table, MemBlock block) {
  size_t val = block.start;
  uint64_t hashed = GetHash(val, table->width_);
  Bucket* bucket = table->table_ + hashed;
  int res = InBucket(bucket, val);
  if (res == -1) {
    bucket->arr =
        (MemBlock*) realloc(bucket->arr, (bucket->len + 1) * sizeof(MemBlock));
    bucket->len++;
    bucket->arr[bucket->len - 1] = block;
  }
}

MemBlock Get(HashTable* table, size_t val) {
  uint64_t hashed = GetHash(val, table->width_);
  Bucket* bucket = table->table_ + hashed;
  int res = InBucket(bucket, val);
  if (res != -1) {
    return bucket->arr[res];
  }
  MemBlock Rub;
  Rub.free = -1;
  return Rub;
}

void Remove(HashTable* table, size_t val) {
  uint64_t hashed = GetHash(val, table->width_);
  Bucket* bucket = table->table_ + hashed;
  int res = InBucket(bucket, val);
  if (res != -1) {
    bucket->arr[res] = bucket->arr[bucket->len - 1];
    bucket->len--;
  }
}

int Exists(HashTable* table, size_t val) {
  uint64_t hashed = GetHash(val, table->width_);
  Bucket* bucket = table->table_ + hashed;
  if (InBucket(bucket, val) == -1) {
    return 0;
  }
  return 1;
}

HashTable* MemTable;
size_t kCapacity;
void* Memory;

extern void myalloc_initialize(int fd) {
  if (fd < 0) {
    return;
  }
  struct stat st;
  fstat(fd, &st);
  uint64_t size = st.st_size;
  Memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  kCapacity = size;
  MemTable = Create(10000);
  MemBlock init;
  init.start = 0;
  init.size = kCapacity;
  init.prev_size = 0;
  init.free = 1;
  Add(MemTable, init);
}

extern void* my_malloc(size_t size) {
  size_t start = 0;
  MemBlock res;
  do {
    res = Get(MemTable, start);
    if (res.free == -1) {
      return 0;
    }
    start = res.start + res.size;
  } while ((res.size < size || res.free == 0) && start < kCapacity);
  if (res.size >= size && res.free == 1) {
    Remove(MemTable, res.start);
    MemBlock new;
    new.start = res.start;
    new.size = size;
    new.prev_size = res.prev_size;
    new.free = 0;
    Add(MemTable, new);
    new.start = res.start + size;
    new.size = res.size - size;
    new.prev_size = size;
    new.free = 1;
    Add(MemTable, new);
    return Memory + res.start;
  }
  return 0;
}

extern void my_free(void* ptr) {
  size_t adr = ptr - Memory;
  size_t freed_size = 0;
  MemBlock next = Get(MemTable, adr);
  size_t prev_s = next.prev_size;
  freed_size += next.size;
  Remove(MemTable, adr);
  next = Get(MemTable, next.start + next.size);
  while (next.free == 1) {
    freed_size += next.size;
    Remove(MemTable, adr);
    next = Get(MemTable, next.start + next.size);
  }
  adr = ptr - Memory - prev_s;
  MemBlock prev = Get(MemTable, adr);
  MemBlock prev_prev;
  size_t move_back = 0;
  while (prev.free == 1) {
    freed_size += prev.size;
    move_back += prev.size;
    Remove(MemTable, adr);
    prev = Get(MemTable, prev.start - prev.prev_size);
  }
  MemBlock new;
  if (prev.free == 0) {
    new.prev_size = prev.size;
  } else {
    new.prev_size = 0;
  }
  new.start = ptr - Memory - move_back;
  new.size = freed_size;
  new.free = 1;
  Add(MemTable, new);
}

extern void myalloc_finalize() {
  msync(Memory, kCapacity, MS_SYNC);
  munmap(Memory, kCapacity);
  Delete(MemTable);
}
