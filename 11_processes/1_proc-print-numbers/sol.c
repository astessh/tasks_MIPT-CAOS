#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  int num = atoi(argv[1]);
  for (int i = 0; i < num; i++) {
    pid_t pid = fork();
    if (pid == 0) {
      printf("%d", i + 1);
      if (i != num-1) {
        printf(" ");
      }
      exit(0);
    }
    waitpid(pid, NULL, 0);
  }
  printf("\n");
  return 0;
}
