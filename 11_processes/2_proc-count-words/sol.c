#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  char word[4097];
  int count = 0;
  while (1) {
    pid_t pid = fork();
    if (pid == 0) {
      int res = scanf("%s", word);
      if (res == -1) {
        exit(0);
      }
      exit(1);
    } else {
      int status;
      waitpid(pid, &status, 0);
      if (status == 0) {
        break;
      }
      count++;
    }
  }
  printf("%d\n", count);
  return 0;
}

