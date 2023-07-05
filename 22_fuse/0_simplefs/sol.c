#define FUSE_USE_VERSION 30

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fuse3/fuse.h>
#include <fuse3/fuse_opt.h>
#include <fuse3/fuse_common.h>

typedef struct {
  char name[NAME_MAX];
  uint64_t offset;
  uint64_t size;
} file_info_t;

typedef struct {
  file_info_t* files;
  uint32_t count;
  char* memory;
  size_t fs_size;
} filesystem_t;

static filesystem_t FS = {0, 0, 0};

void open_filesystem(const char* file_name) {
  int fd = open(file_name, O_RDONLY);
  if (fd == -1) {
    perror("Can't open FS image");
    exit(1);
  }
  struct stat st;
  fstat(fd, &st);
  FS.fs_size = st.st_size;
  FS.memory = mmap(NULL, FS.fs_size, PROT_READ, MAP_PRIVATE, fd, 0);
  char* new_line = strstr(FS.memory, "\n\n");
  size_t offset = new_line - FS.memory + 2;
  FILE* stream = fdopen(fd, "r");
  fscanf(stream, "%u", &FS.count);
  if (FS.count > 0) {
    FS.files = calloc(FS.count, sizeof(*FS.files));
  }
  for (uint32_t i = 0; i < FS.count; ++i) {
    file_info_t* fi = &FS.files[i];
    fscanf(stream, "%s %lu", fi->name, &fi->size);
    fi->offset = offset;
    offset += fi->size;
  }
  close(fd);
}

void close_filesystem() {
  if (FS.count > 0)
    free(FS.files);
  FS.count = 0;
  FS.files = NULL;
  munmap(FS.memory, FS.fs_size);
  FS.memory = NULL;
  FS.fs_size = 0;
}

const file_info_t* find_file(const char* path) {
  path += 1;
  file_info_t* my_fi = NULL;
  file_info_t* target = NULL;
  for (uint32_t i = 0; i < FS.count; ++i) {
    my_fi = &FS.files[i];
    if (0 == strncmp(my_fi->name, path, NAME_MAX)) {
      target = my_fi;
      break;
    }
  }
  return target;
}

int my_stat(const char* path, struct stat* st, struct fuse_file_info* fi) {
  if (0 == strcmp("/", path)) {
    st->st_mode = 0555 | S_IFDIR;
    st->st_nlink = 2;
    return 0;
  }

  const file_info_t* target = find_file(path);
  if (!target) {
    return -ENOENT;
  }
  st->st_nlink = 1;
  st->st_mode = S_IFREG | 0444;
  st->st_size = target->size;
  return 0;
}

int my_readdir(const char* path, void* out, fuse_fill_dir_t filler, off_t off,
               struct fuse_file_info* fi, enum fuse_readdir_flags flags) {
  if (0 != strcmp(path, "/")) {
    return -ENOENT;
  }
  filler(out, ".", NULL, 0, 0);
  filler(out, "..", NULL, 0, 0);
  file_info_t* my_fi = NULL;
  for (uint32_t i = 0; i < FS.count; ++i) {
    my_fi = &FS.files[i];
    filler(out, my_fi->name, NULL, 0, 0);
  }
  return 0;
}

int my_open(const char* path, struct fuse_file_info* fi) {
  const file_info_t* target = find_file(path);
  if (!target) {
    return -ENOENT;
  }
  if (O_RDONLY != (fi->flags & O_ACCMODE)) {
    return -EACCES;
  }
  return 0;
}

int my_read(const char* path, char* out, size_t size, off_t off,
            struct fuse_file_info* fi) {
  const file_info_t* target = find_file(path);
  if (size - off > target->size - off) {
    size = target->size - off;
  }
  if (size > 0) {
    char* begin = FS.memory + target->offset + off;
    memcpy(out, begin, size);
  }
  return size;
}

struct fuse_operations operations = {
    .readdir = my_readdir,
    .getattr = my_stat,
    .open    = my_open,
    .read    = my_read,
};

typedef struct { char* src; } my_options_t;
my_options_t my_options;
struct fuse_opt opt_specs[] = {
    {"--src %s", offsetof(my_options_t, src), 0},
    {NULL, 0, 0}
};

int main(int argc, char* argv[]) {
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
  fuse_opt_parse(&args, &my_options, opt_specs, NULL);
  open_filesystem(my_options.src);
  int ret = fuse_main(args.argc, args.argv, &operations, NULL);
  close_filesystem();
  return ret;
}
