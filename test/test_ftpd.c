#include "bscl_ftpd.h"
#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <direct.h>
#include "bscl_os.h"

#define BASE_PATH "c:"
#ifndef PATH_MAX
#define PATH_MAX 256
#endif

static void get_full_path(char *full_path, const char *path) {
  if (path[0] == '/') {
    snprintf(full_path, PATH_MAX, BASE_PATH "%s", path);
  } else {
    // relative path
    strncpy(full_path, path, PATH_MAX);
  }
}

static int vfs_open(const char *path, uint8_t mode) {
  char full_path[PATH_MAX] = BASE_PATH;
  get_full_path(full_path, path);
  return os_fs_open(full_path, mode);
}

static int vfs_write(int fd, const void *buf, int len) {
  return os_fs_write(fd, buf, len);
}

static int vfs_read(int fd, void *buf, int len) {
  return os_fs_read(fd, buf, len);
}
static int vfs_fstat(int fd, struct os_fs_stat_buf *buf) {
  return os_fs_fstat(fd, buf);
}
static int vfs_stat(const char *path, struct os_fs_stat_buf *buf) {
  char full_path[PATH_MAX] = BASE_PATH;
  get_full_path(full_path, path);
  return os_fs_stat(full_path, buf);
}

static int vfs_close(int fd) {
  return os_fs_close(fd);
}

static int vfs_mkdir(const char *path) {
  char full_path[PATH_MAX] = BASE_PATH;
  get_full_path(full_path, path);
  return os_fs_mkdir(full_path);
}

static int vfs_opendir(const char *path) {
  char full_path[PATH_MAX] = BASE_PATH;
  get_full_path(full_path, path);
  return os_fs_opendir(full_path);
}

static int vfs_readdir(int fd, struct os_fs_dirent *dirent) {
  return os_fs_readdir(fd, dirent);
}

static int vfs_closedir(int fd) {
  return os_fs_closedir(fd);
}

static int vfs_remove(const char *path) {
  char full_path[PATH_MAX] = BASE_PATH;
  get_full_path(full_path, path);
  return os_fs_remove(path);
}

static int vfs_chdir(const char *path) {
  char full_path[PATH_MAX] = BASE_PATH;
  get_full_path(full_path, path);
  return chdir(full_path);
}

static char *vfs_getcwd(char *path, int size) {
  char full_path[PATH_MAX];
  getcwd(full_path, PATH_MAX);
  for (int i = 0; i < PATH_MAX && full_path[i] != '\0'; ++i) {
    if (full_path[i] == '\\') {
      full_path[i] = '/';
    }
  }
  strncpy(path, full_path + sizeof(BASE_PATH) - 1, size);
  return path;
}

const ftpd_callback_t callback = {
    .open = vfs_open,
    .read = vfs_read,
    .write = vfs_write,
    .close = vfs_close,
    .opendir = vfs_opendir,
    .readdir = vfs_readdir,
    .closedir = vfs_closedir,
    .mkdir = vfs_mkdir,
    .remove = vfs_remove,
    .chdir = vfs_chdir,
    .getcwd = vfs_getcwd,
    .stat = vfs_stat,
    .fstat = vfs_fstat,
};

int main(int argc, char *argv[]) {
  chdir(BASE_PATH);
  ftpd_run(21, &callback);
  return 0;
}
