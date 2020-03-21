#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

bool os_fs_exists(const char *path) {
  struct stat attr;
  return lstat(path, &attr) == 0;
}

bool os_fs_isdir(const char *path) {
  struct stat attr;
  if (lstat(path, &attr) != 0)
    return false;
  return S_ISDIR(attr.st_mode);
}

int64 os_fs_mtime(const char *path) {
  struct stat attr;
  if (lstat(path, &attr) != 0)
    return -1;
  return attr.st_mtime;
}

int64 os_fs_fsize(const char *path) {
  struct stat attr;
  if (lstat(path, &attr) != 0)
    return -1;
  return attr.st_size;
}

// p = false  ->  mkdir
// p = true   ->  mkdir -p
bool os_fs_mkdir(const char *path) {
  return mkdir(path, 0755) == 0;
}

// rf = false  ->  rm or rmdir
// rf = true   ->  rm -rf
bool os_fs_remove(const char *path) {
  if (!os_fs_exists(path))
    return true;

  if (os_fs_isdir(path))
    return rmdir(path) == 0;
  else
    return unlink(path) == 0;
}

bool os_fs_rename(const char *from, const char *to) {
  return rename(from, to) == 0;
}

bool os_fs_symlink(const char *dst, const char *lnk) {
  remove(lnk);
  return symlink(dst, lnk) == 0;
}
