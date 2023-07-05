#include <limits.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>

int main() {
  char in[4096];
  char path_link[4096];
  char rel[4096];
  char out[4096];
  while (fgets(in, sizeof(in), stdin)) {
    *(in - 1 + strlen(in)) = '\0';
    struct stat st;
    int read_status = lstat(in, &st);
    if (read_status != -1) {
      if (S_ISREG(st.st_mode)) {
        char* base_name = basename(in);
        char link_to_[4096] = "link_to_";
        strcat(link_to_, base_name);
        symlink(in, link_to_);
      } else if (S_ISLNK(st.st_mode)) {
        int link = readlink(in, path_link, sizeof(path_link));
        *(path_link + link) = '\0';
        sprintf(rel, "%s/%s", dirname(in), path_link);
        realpath(rel, out);
        printf("%s\n", out);
      }
    }
  }
  return 0;
}

