#include <sys/syscall.h>

extern long syscall(long number, ...);

int _start() {
  char sym;
  while (syscall(SYS_read, 0, &sym, 1) != 0) {
    syscall(SYS_write, 1, &sym, 1);
  }
  syscall(SYS_exit, 0);
}
