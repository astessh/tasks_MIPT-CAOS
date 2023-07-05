#include <sys/syscall.h>
long syscall(long number, ...);
int _start () {
	char* word = "Hello, World!\n";
	syscall(SYS_write,1, word, 14);
	syscall(SYS_exit, 0);
}

