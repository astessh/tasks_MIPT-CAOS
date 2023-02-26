/*
Программе передаётся произвольное количество аргументов: CMD1, CMD2, ..., CMDN.

Необходимо реализовать эквивалент запуска их командной строки: CMD1 | CMD2 | ... | CMDN.

Родительский процесс должен завершаться самым последним!
*/

#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void StartProcess(char* argv[], int i, int prev_out) {
  if (argv[i + 1] != NULL) {
    if (i != 0) {
      dup2(prev_out, 0);
      close(prev_out);
    }
    int fds_pair[2];
    pipe(fds_pair);
    pid_t pid = fork();
    if (pid == 0) {
      if (argv[i + 2] != NULL) {
        dup2(fds_pair[1], 1);
      }
      close(fds_pair[0]);
      close(fds_pair[1]);
      execlp(argv[i + 1], argv[i + 1], NULL);
    }
    close(fds_pair[1]);
    StartProcess(argv, i + 1, fds_pair[0]);
    waitpid(pid, 0, 0);
  }
}

int main(int argc, char* argv[]) {
  StartProcess(argv, 0, 0);
  return 0;
}
