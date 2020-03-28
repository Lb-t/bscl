#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include "bscl_fs.h"
#include <dirent.h>
#include <stdio.h>

bool bscl_fs_exists(const char *path) {
  struct stat attr;
  return lstat(path, &attr) == 0;
}

bool bscl_fs_isdir(const char *path) {
  struct stat attr;
  if (lstat(path, &attr) != 0)
    return false;
  return S_ISDIR(attr.st_mode);
}

int64_t bscl_fs_mtime(const char *path) {
  struct stat attr;
  if (lstat(path, &attr) != 0)
    return -1;
  return attr.st_mtime;
}

int64_t bscl_fs_fsize(const char *path) {
  struct stat attr;
  if (lstat(path, &attr) != 0)
    return -1;
  return attr.st_size;
}

// p = false  ->  mkdir
// p = true   ->  mkdir -p
bool bscl_fs_mkdir(const char *path) {
  return mkdir(path, 0755) == 0;
}

// rf = false  ->  rm or rmdir
// rf = true   ->  rm -rf
bool bscl_fs_remove(const char *path) {
  if (!bscl_fs_exists(path))
    return true;

  if (bscl_fs_isdir(path))
    return rmdir(path) == 0;
  else
    return unlink(path) == 0;
}

bool bscl_fs_rename(const char *from, const char *to) {
  return rename(from, to) == 0;
}

bool bscl_fs_symlink(const char *dst, const char *lnk) {
  remove(lnk);
  return symlink(dst, lnk) == 0;
}

int bscl_fs_open(const char *path, uint8_t mode) {
  return open(path, mode);
}
int bscl_fs_write(int fd, const void *buf, int len) {
  return write(fd, buf, len);
}
int bscl_fs_read(int fd, void *buf, int len) {
  return read(fd, buf, len);
}
int bscl_fs_seek(int fd, int offset, uint8_t origin) {
  return lseek(fd, offset, origin);
}
int bscl_fs_tell(int fd) {
  return lseek(fd, 0, BSCL_FS_SEEK_CUR);
}
int bscl_fs_fstat(int fd, struct bscl_fs_stat_buf *buf) {
  return -1;
}
int bscl_fs_stat(const char *path, struct bscl_fs_stat_buf *buf) {
  struct stat stat_buf;
  int res = stat(path, &stat_buf);
  // todo:fill the struct
  return res;
}
int bscl_fs_flush(int fd) {
  return fsync(fd);
}
int bscl_fs_close(int fd) {
  return close(fd);
}

int bscl_fs_eof(int fd) {
  return -1;
}

int bscl_fs_opendir(const char *path) {
  return -1;
}
int bscl_fs_readdir(int fd, struct bscl_fs_dirent *dirent) {
  return -1;
}
int bscl_fs_closedir(int fd) {
  return -1;
}
