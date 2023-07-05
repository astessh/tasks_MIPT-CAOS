#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

typedef struct MemBlock {
  uint32_t size;
  char free;
} MemBlock;

uint32_t kCapacity;
void* Memory;
uint32_t size_of_MemBlock = 5;

extern void myalloc_initialize(int fd) {
  if (fd < 0) {
    return;
  }
  struct stat st;
  fstat(fd, &st);
  uint32_t size = st.st_size;
  Memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  kCapacity = size;
  MemBlock init;
  init.size = kCapacity - size_of_MemBlock;
  init.free = 1;
  *(MemBlock*) (Memory) = init;
}

extern void* my_malloc(size_t size) {
  uint32_t start = 0;
  MemBlock res;
  do {
    res = *(MemBlock*) (Memory + start);
    start += res.size + size_of_MemBlock;
  } while ((res.size < size || res.free == 0)
      && start + size + size_of_MemBlock <= kCapacity);
  if (res.size >= size && res.free == 1) {
    if (res.size - size >= size_of_MemBlock) {
      MemBlock new;
      new.size = size;
      new.free = 0;
      *(MemBlock*) (Memory + start - res.size - size_of_MemBlock) = new;
      new.size = res.size - size - size_of_MemBlock;
      new.free = 1;
      *(MemBlock*) (Memory + start - res.size + size) = new;
    } else {
      MemBlock new;
      new.size = res.size;
      new.free = 0;
      *(MemBlock*) (Memory + start - res.size - size_of_MemBlock) = new;
    }
    return Memory + start - res.size;
  }
  return 0;
}

extern void my_free(void* ptr) {
  uint32_t adr = ptr - Memory - size_of_MemBlock;
  MemBlock cur = *(MemBlock*) (Memory + adr);
  uint32_t freed_forward = cur.size;
  adr += cur.size + size_of_MemBlock;
  cur = *(MemBlock*) (Memory + adr);
  while (adr < kCapacity && cur.free == 1) {
    freed_forward += cur.size + size_of_MemBlock;
    adr += cur.size + size_of_MemBlock;
    cur = *(MemBlock*) (Memory + adr);
  }
  uint32_t target_adr = ptr - Memory - size_of_MemBlock;
  adr = 0;
  uint32_t freed_backward = 0;
  while (adr < target_adr) {
    cur = *(MemBlock*) (Memory + adr);
    if (cur.free == 1) {
      freed_backward += cur.size + size_of_MemBlock;
    } else {
      freed_backward = 0;
    }
    adr += cur.size + size_of_MemBlock;
  }
  adr -= freed_backward;
  MemBlock new;
  new.size = freed_backward + freed_forward;
  new.free = 1;
  *(MemBlock*) (Memory + adr) = new;
}

extern void myalloc_finalize() {
  msync(Memory, kCapacity, MS_SYNC);
  munmap(Memory, kCapacity);
}
