#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

const char pattern[] =
    "#include <stdio.h>\n\nint main() {\nint expr = (%s);\nprintf(\"%%d\", expr);\nreturn 0;\n}\n";

int main() {
  char input[4096];
  char code[4096];
  fgets(input, sizeof(input), stdin);
  char* slash = strchr(input, '\n');
  if (slash) *slash = '\0';
  if (strnlen(input, sizeof(input)) == 0) return 0;
  snprintf(code, sizeof(code), pattern, input);
  int file = open("_temp_.c", O_RDWR | O_CREAT | O_TRUNC, 0644);
  write(file, code, strlen(code));
  close(file);
  pid_t pid = fork();
  if (pid == 0) {
    execlp("gcc", "gcc", "-o", "_temp_", "_temp_.c", NULL);
    perror("failed to compile");
  }
  waitpid(pid, NULL, 0);
  pid = fork();
  if (pid == 0) {
    execl("./_temp_", "./_temp_", NULL);
    perror("failed to execute");
  }
  waitpid(pid, NULL, 0);
  remove("_temp_");
  remove("_temp_.c");
  return 0;
}

