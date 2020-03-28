#pragma once
#include <stdint.h>
#include <time.h>
#include <stdbool.h>

#define BSCL_FS_PATH_MAX 128

#define BSCL_FS_TYPE_DIR 0 // directory
#define BSCL_FS_TYPE_REG 1 // regular file

/*nffs open mode*/
#define BSCL_FS_MODE_WRITE (uint8_t)2
#define BSCL_FS_MODE_READ (uint8_t)1
#define BSCL_FS_MODE_CREATE (uint8_t)4 /* if not exist,then create one */
#define BSCL_FS_MODE_TRUNC (uint8_t)8  /*if file exist ,then delete it and create a new file*/

/*nffs seek*/
#define BSCL_FS_SEEK_SET 0 /* set file offset to offset */
#define BSCL_FS_SEEK_CUR 1 /* set file offset to current plus offset */
#define BSCL_FS_SEEK_END 2 /* set file offset to EOF plus offset */

struct bscl_fs_stat_buf {
  uint8_t type;
  uint32_t size;
  time_t time;
};

struct bscl_fs_dirent {
  uint8_t type;
  char name[BSCL_FS_PATH_MAX];
};

bool bscl_fs_exists(const char *path);
bool bscl_fs_isdir(const char *path);
int64_t bscl_fs_mtime(const char *path);
int64_t bscl_fs_fsize(const char *path);

int bscl_fs_open(const char *path, uint8_t mode);
int bscl_fs_write(int fd, const void *buf, int len);
int bscl_fs_read(int fd, void *buf, int len);
int bscl_fs_seek(int fd, int offset, uint8_t origin);
int bscl_fs_tell(int fd);
int bscl_fs_fstat(int fd, struct bscl_fs_stat_buf *buf);
int bscl_fs_stat(const char *path, struct bscl_fs_stat_buf *buf);
int bscl_fs_flush(int fd);
int bscl_fs_close(int fd);
int bscl_fs_eof(int fd);

bool bscl_fs_mkdir(const char *path);
int bscl_fs_opendir(const char *path);
int bscl_fs_readdir(int fd, struct bscl_fs_dirent *dirent);
int bscl_fs_closedir(int fd);

bool bscl_fs_remove(const char *path);
bool bscl_fs_rename(const char *from, const char *to);
bool bscl_fs_symlink(const char *dst, const char *lnk);
