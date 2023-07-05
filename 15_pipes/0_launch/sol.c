#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char* argv[]) {
  char* cmd = argv[1];
  char* file = argv[2];
  int in = open(file, O_RDONLY);
  dup2(in, 0);
  close(in);
  int fds_pair[2];
  pipe(fds_pair);
  pid_t pid = fork();
  if (pid == 0) {
    dup2(fds_pair[1], 1);
    close(fds_pair[1]);
    execlp(cmd, cmd, NULL);
  } else {
    close(fds_pair[1]);
    uint64_t total_count = 0;
    size_t count;
    char buff[4096];
    while ((count = read(fds_pair[0], buff, sizeof(buff))) > 0) {
      total_count += count;
    }
    waitpid(pid, 0, 0);
    printf("%lu\n", total_count);
  }
}
