#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int NotCounted(const size_t* array, size_t size, size_t current) {
  for (size_t i = 0; i < size; i++) {
    if (array[i] == current) {
      return 0;
    }
  }
  return 1;
}

int main(int argc, char* argv[]) {
  const char gcc[] = "gcc";
  const char* name = argv[1];
  const char war[] = "warning";
  const char err[] = "error";
  int fds_pair[2];
  pipe(fds_pair);
  pid_t pid = fork();
  if (pid == 0) {
    dup2(fds_pair[1], 2);
    close(fds_pair[0]);
    close(fds_pair[1]);
    execlp(gcc, gcc, name, NULL);
  }
  close(fds_pair[1]);
  dup2(fds_pair[0], 0);
  size_t row;
  size_t* warnings = (size_t*) malloc(0);
  size_t* errors = (size_t*) malloc(0);
  size_t warnings_count = 0;
  size_t errors_count = 0;
  char str[4096];
  char str2[4096];
  while (scanf("%s", str) > 0) {
    char* name_found = strstr(str, name);
    if (name_found != NULL) {
      name_found += sizeof(name) - 2;
      if (scanf("%s", str2) > 0) {
        char* war_found = strstr(str2, war);
        if (war_found != NULL) {
          sscanf(name_found, ":%lu:*%lu:", &row, &row);
          if (NotCounted(warnings, warnings_count, row) == 1) {
            warnings_count++;
            warnings =
                (size_t*) realloc(warnings, warnings_count * sizeof(size_t));
            warnings[warnings_count - 1] = row;
          }
        }
        char* err_found = strstr(str2, err);
        if (err_found != NULL) {
          sscanf(name_found, ":%lu:*%lu:", &row, &row);
          if (NotCounted(errors, errors_count, row) == 1) {
            errors_count++;
            errors = (size_t*) realloc(errors, errors_count * sizeof(size_t));
            errors[errors_count - 1] = row;
          }
        }
      }
    }
  }
  close(fds_pair[1]);
  close(fds_pair[0]);
  waitpid(pid, 0, 0);
  free(warnings);
  free(errors);
  printf("%lu %lu\n", errors_count, warnings_count);
  return 0;
}
