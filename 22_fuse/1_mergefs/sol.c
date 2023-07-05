#define FUSE_USE_VERSION 30

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>
#include <fuse.h>

const size_t kMaxNumberOfFiles = 100;
const size_t kMaxPathSize = 500;

typedef struct {
  size_t directories_number;
  char directories[100][500];
} f_sys_t;

static f_sys_t f_sys;

typedef struct {
  char path_[500];
  struct stat stat_;
  int if_directory;
  int error_code;
} file_t;

file_t find(const char* path) {
  char path_[kMaxPathSize];
  struct stat stat_;
  file_t my_file;
  my_file.error_code = ENOENT;
  my_file.if_directory = 0;
  int exists = 0;
  for (uint32_t i = 0; i < f_sys.directories_number; i += 1) {
    sprintf(path_, "%s%s", f_sys.directories[i], path);
    if (0 == stat(path_, &stat_)) {
      if (stat_.st_mtime > my_file.stat_.st_mtime || 0 == exists) {
        my_file.if_directory = (0 != (stat_.st_mode & S_IFDIR));
        my_file.error_code = 0;
        sprintf(my_file.path_, "%s", path_);
        my_file.stat_ = stat_;
        exists = 1;
      }
    } else if (0 == exists) {
      my_file.error_code = errno;
    }
  }
  return my_file;
}

int my_open(const char* path, struct fuse_file_info* fi) {
  file_t my_file = find(path);
  if (my_file.error_code) {
    return -my_file.error_code;
  }
  if ((fi->flags & O_ACCMODE) != O_RDONLY) {
    return -EACCES;
  }
  int file_fd;
  if (-1 == (file_fd = open(my_file.path_, O_RDONLY))) {
    return -errno;
  } else {
    fi->fh = file_fd;
    return 0;
  }
}

int my_release(const char* path, struct fuse_file_info* fi) {
  ssize_t my_result = close((int) fi->fh);
  if (-1 != my_result) {
    return 0;
  } else {
    return -errno;
  }
}

int my_stat(const char* path, struct stat* st, struct fuse_file_info* fi) {
  if (0 == strcmp("/", path)) {
    st->st_nlink = 2;
    st->st_mode = 0555 | S_IFDIR;
    return 0;
  }
  file_t file = find(path);
  if (file.error_code) {
    return -file.error_code;
  }
  if (!file.if_directory) {
    *st = file.stat_;
    st->st_mode = S_IFREG | 0444;
  } else {
    *st = file.stat_;
    st->st_mode = S_IFDIR | 0555;
  }
  return 0;
}

int check_new_file(char files_dict[kMaxNumberOfFiles][kMaxPathSize],
                   char* path) {
  for (size_t i = 0; i < kMaxNumberOfFiles; i += 1) {
    if (!files_dict[i][0]) {
      strcpy(files_dict[i], path);
      return 1;
    } else if (0 == strncmp(files_dict[i], path, strlen(path) + 1)) {
      return 0;
    }
  }
  return 0;
}

int my_readdir(const char* path, void* out, fuse_fill_dir_t filler, off_t off,
               struct fuse_file_info* fi, enum fuse_readdir_flags flags) {
  char path_[kMaxPathSize], files_dict[kMaxNumberOfFiles][kMaxPathSize];
  strcpy(path_, path);
  filler(out, ".", NULL, 0, 0);
  filler(out, "..", NULL, 0, 0);
  if (1 == strlen(path_)) {
    path_[0] = 0;
  }
  for (uint32_t i = 0; i < f_sys.directories_number; i += 1) {
    char cwd[kMaxPathSize], base_path[kMaxPathSize];
    getcwd(cwd, kMaxPathSize);
    sprintf(base_path, "%s%s", f_sys.directories[i], path_);
    struct dirent* entity;
    DIR* dir;
    if (NULL != (dir = opendir(base_path))) {
      while (NULL != (entity = readdir(dir))) {
        if ('.' != entity->d_name[0]
            && check_new_file(files_dict, entity->d_name)) {
          filler(out, entity->d_name, NULL, 0, 0);
        }
      }
      closedir(dir);
    }
  }
  return 0;
}

int my_read(const char* path,
            char* out,
            size_t size,
            off_t off,
            struct fuse_file_info* file_info) {
  ssize_t res = read((int) file_info->fh, out, size);
  if (-1 != res) {
    return (int) res;
  } else {
    return -errno;
  }
}

typedef struct {
  char* src;
} custom_options_t;

struct fuse_operations custom_operations = {
    .open    = my_open,
    .release = my_release,
    .readdir = my_readdir,
    .getattr = my_stat,
    .read    = my_read,
};

struct fuse_opt opt_specifications[] = {
    {"--src %s", offsetof(custom_options_t, src), 0},
    {NULL, 0, 0}
};
custom_options_t custom_options;

int main(int argc, char* argv[]) {
  struct fuse_args f_args = FUSE_ARGS_INIT(argc, argv);
  fuse_opt_parse(&f_args, &custom_options, opt_specifications, NULL);
  char cwd[kMaxPathSize];
  char* ptr = strtok(custom_options.src, ":");
  getcwd(cwd, kMaxPathSize);
  size_t count = 0;
  while (ptr) {
    sprintf(f_sys.directories[count], "%s/%s", cwd, ptr);
    count += 1;
    ptr = strtok(0, ":");
  }
  f_sys.directories_number = count;
  int ret = fuse_main(f_args.argc,
                      f_args.argv,
                      &custom_operations,
                      NULL);
  f_sys.directories_number = 0;
  return ret;
}

