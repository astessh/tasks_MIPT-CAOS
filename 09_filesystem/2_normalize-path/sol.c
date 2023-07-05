extern void normalize_path(char* path) {
  char* read = path;
  char* write = path;
  char* prev = path - 1;
  while (*read > '\0') {
    if (*read == '/' && *(read + 1) == '/') {
      read++;
      continue;
    }
    if (*read == '.' && *(read + 1) == '/') {
      read += 2;
      continue;
    }
    if (*read == '.' && *(read + 1) == '.' && *(read + 2) == '/') {
      read += 3;
      write = prev + 1;
    }
    if (*read == '/' && *(read + 1) != '.') {
      prev = read;
    }
    if (write != read) {
      *write = *read;
    }
    read++;
    write++;
  }
  *write = *read;
}

