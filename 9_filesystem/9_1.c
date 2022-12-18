/*Программе на стандартном потоке ввода передаётся список имён файлов.

Необходимо вывести на стандартный поток вывода имена "неправильных" выполняемых файлов, то есть файлов, которые отмечены как исполняемые, но при этом они заведомо не могут быть выполнены в системе (не начинаются с корректной строки #!, содержащей запускаемый интерпретатор, и не являются ELF-файлами).*/

#include <sys/stat.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int CheckForELF(int file) {
  char buff[4];
  lseek(file, 0, SEEK_SET);
  read(file, &buff, sizeof(buff));
  if (buff[0] == 127 && buff[1] == 'E' && buff[2] == 'L' && buff[3] == 'F') {
    return 1;
  }
  return 0;
}

int CheckForSheba(int file) {
  char buff[2];
  lseek(file, 0, SEEK_SET);
  read(file, &buff, sizeof(buff));
  if (buff[0] == '#' && buff[1] == '!') {
    char path[PATH_MAX];
    struct stat st;
    if (read(file, &path, sizeof(path))) {
      strtok(path, "\n");
      if (lstat(path, &st) != -1 && S_ISREG(st.st_mode)) {
        if (access(path, X_OK) == 0) {
          return 1;
        }
      }
    }
    return 0;
  }
  return 0;
}

int main(int argc, char* argv[]) {
  char file_name[PATH_MAX];
  struct stat st;
  while (fgets(file_name, PATH_MAX, stdin) != NULL) {
    strtok(file_name, "\n");
    if (lstat(file_name, &st) != -1 && S_ISREG(st.st_mode)) {
      if (access(file_name, X_OK) == 0) {
        int file = open(file_name, O_RDONLY);
        if (CheckForELF(file) == 0 && CheckForSheba(file) == 0) {
          printf("%s", file_name);
        }
        close(file);
      }
    }
  }
}

