#pragma once
#include <stdint.h>
#include <time.h>

#define OS_FS_PATH_MAX 128

#define OS_FS_TYPE_DIR 0 // directory
#define OS_FS_TYPE_REG 1 // regular file

/*nffs open mode*/
#define OS_FS_MODE_WRITE (uint8_t)2
#define OS_FS_MODE_READ (uint8_t)1
#define OS_FS_MODE_CREATE (uint8_t)4 /* if not exist,then create one */
#define OS_FS_MODE_TRUNC (uint8_t)8  /*if file exist ,then delete it and create a new file*/

/*nffs seek*/
#define OS_FS_SEEK_SET 0 /* set file offset to offset */
#define OS_FS_SEEK_CUR 1 /* set file offset to current plus offset */
#define OS_FS_SEEK_END 2 /* set file offset to EOF plus offset */

struct os_fs_stat_buf {
    uint8_t type;
    uint32_t size;
    time_t time;
};

struct os_fs_dirent {
    uint8_t type;
    char name[OS_FS_PATH_MAX];
};

int os_fs_open(const char *path, uint8_t mode);
int os_fs_write(int fd, const void *buf, int len);
int os_fs_read(int fd, void *buf, int len);
int os_fs_seek(int fd, int offset, uint8_t origin);
int os_fs_tell(int fd);
int os_fs_fstat(int fd, struct os_fs_stat_buf *buf);
int os_fs_stat(const char *path, struct os_fs_stat_buf *buf);
int os_fs_flush(int fd);
int os_fs_close(int fd);
int os_fs_eof(int fd);

int os_fs_mkdir(const char *path);
int os_fs_opendir(const char *path);
int os_fs_readdir(int fd, struct os_fs_dirent *dirent);
int os_fs_closedir(int fd);

int os_fs_remove(const char *path);
