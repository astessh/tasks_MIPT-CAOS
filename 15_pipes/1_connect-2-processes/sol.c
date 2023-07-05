#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char* argv[]) {
  char* cmd1 = argv[1];
  char* cmd2 = argv[2];
  int fds_pair[2];
  pipe(fds_pair);
  pid_t pid1 = fork();
  if (pid1 == 0) {
    dup2(fds_pair[1], 1);
    close(fds_pair[0]);
    close(fds_pair[1]);
    execlp(cmd1, cmd1, NULL);
  }
  pid_t pid2 = fork();
  if (pid2 == 0) {
    dup2(fds_pair[0], 0);
    close(fds_pair[0]);
    close(fds_pair[1]);
    execlp(cmd2, cmd2, NULL);
  }
  close(fds_pair[1]);
  close(fds_pair[0]);
  waitpid(pid1, 0, 0);
  waitpid(pid2, 0, 0);
  return 0;
}
