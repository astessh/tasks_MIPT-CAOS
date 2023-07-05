#define FUSE_USE_VERSION 30

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse3/fuse.h>
#include <fuse3/fuse_common.h>
#include <fuse3/fuse_lowlevel.h>
#include <fuse3/fuse_opt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <zip.h>

const size_t kBufSize = 1 << 20;
const size_t kNameLen = 1 << 12;

typedef struct {
  char buf[1 << 20];
  char name[1 << 12];
  zip_t* archive;
} data_t;

data_t* data() {
  static data_t data;
  return &data;
}

char* get_path_by_id(int id) {
  zip_t* archive = data()->archive;
  char* name = data()->name;
  name[0] = '/';
  name[1] = '\0';
  strcat(name, zip_get_name(archive, id, 0));
  return name;
}

void open_f_sys(const char* image_filename) {
  data()->archive = zip_open(image_filename, ZIP_RDONLY, NULL);
}

char* get_name(const char* path) {
  zip_t* archive = data()->archive;
  if (0 == strcmp(path, "/")) {
    char* name = data()->name;
    name[0] = '/';
    name[1] = '\0';
    return name;
  }
  zip_int64_t num_entries = zip_get_num_entries(archive, 0);
  for (zip_int64_t i = 0; i < num_entries; ++i) {
    char* name = get_path_by_id(i);
    if (0 != strcmp(name, path)) {
      int namelen = strlen(name);
      if (strlen(path) + 1 != namelen || name[namelen - 1] != '/' ||
          0 != strncmp(path, name, namelen - 1))
        continue;
    }
    return name;
  }
  return NULL;
}

int my_open(const char* path, struct fuse_file_info* fi) {
  if (0 == strcmp(path, "/")) {
    if (O_RDONLY != (fi->flags & O_ACCMODE))
      return -EACCES;
    return 0;
  }
  const char* name = get_name(path);
  if (!name)
    return -ENOENT;
  if (O_RDONLY != (fi->flags & O_ACCMODE))
    return -EACCES;
  return 0;
}

int my_read(
    const char* path,
    char* out,
    size_t size,
    off_t off,
    struct fuse_file_info* fi) {
  zip_t* archive = data()->archive;
  if (strlen(path) == 0 || path[0] != '/')
    return -ENOENT;
  if (0 == strcmp(path, "/"))
    return -EACCES;
  const char* name = get_name(path);
  if (name[strlen(name) - 1] == '/')
    return -EACCES;
  zip_file_t* file = zip_fopen(archive, name + 1, 0);
  if (file == NULL) {
    return -EACCES;
  }
  char* buf = data()->buf;
  zip_int64_t was_read = zip_fread(file, buf, off + size);
  if (was_read < 0)
    return -EACCES;
  if (was_read <= off)
    return 0;
  size = was_read - off;
  memcpy(out, buf + off, size);
  return size;
}

int my_readdir(
    const char* path,
    void* out,
    fuse_fill_dir_t filler,
    off_t off,
    struct fuse_file_info* fi,
    enum fuse_readdir_flags flags) {
  zip_t* archive = data()->archive;
  if (strlen(path) == 0 || path[0] != '/')
    return -ENOENT;
  int open_flags = my_open(path, fi);
  if (open_flags != 0)
    return open_flags;
  char* dirname = data()->buf;
  dirname[0] = '\0';
  char* tmp = get_name(path);
  strcat(dirname, tmp);
  size_t dirlen = strlen(dirname);
  if (dirname[strlen(dirname) - 1] != '/')
    return -EACCES;
  filler(out, ".", NULL, 0, 0);
  filler(out, "..", NULL, 0, 0);

  zip_int64_t num_entries = zip_get_num_entries(archive, 0);

  for (zip_int64_t i = 0; i < num_entries; i++) {
    char* name = get_path_by_id(i);
    if (0 != strncmp(name, dirname, dirlen))
      continue;
    if (strlen(name) == dirlen)
      continue;
    char* next_dir = strstr(name + dirlen, "/");
    if (next_dir == NULL || *(next_dir + 1) == '\0') {
      if (next_dir)
        *next_dir = '\0';
      filler(out, name + dirlen, NULL, 0, 0);
    }
  }
  return 0;
}

int my_stat(const char* path, struct stat* st, struct fuse_file_info* fi) {
  zip_t* archive = data()->archive;
  if (strlen(path) == 0 || path[0] != '/')
    return -ENOENT;
  if (0 == strcmp(path, "/")) {
    st->st_nlink = 2;
    st->st_mode = (0555 | S_IFDIR);
    return 0;
  }
  const char* name = get_name(path);
  if (!name)
    return -ENOENT;
  if (name[strlen(name) - 1] == '/') {
    st->st_nlink = 2;
    st->st_mode = (0555 | S_IFDIR);
    return 0;
  }
  struct zip_stat zst;
  if (zip_stat(archive, name + 1, ZIP_STAT_SIZE, &zst) != 0) {
    return -EACCES;
  }
  st->st_size = zst.size;
  st->st_nlink = 1;
  st->st_mode = (0444 | S_IFREG);
  return 0;
}

typedef struct {
  char* src;
} my_options_t;

int main(int argc, char* argv[]) {
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
  my_options_t opts;
  memset(&opts, 0, sizeof(opts));
  struct fuse_opt opt_specs[] = {{"--src %s", 0, 0}, {NULL, 0, 0}};
  fuse_opt_parse(&args, &opts, opt_specs, NULL);
  if (!opts.src) {
    exit(1);
  } else {
    open_f_sys(opts.src);
  }
  struct fuse_operations operations = {
      .open = my_open,
      .read = my_read,
      .readdir = my_readdir,
      .getattr = my_stat,
  };
  return fuse_main(args.argc, args.argv, &operations, NULL);
}
