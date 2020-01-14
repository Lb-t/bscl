#pragma once
#include <stdint.h>
#include "bscl_os.h"

typedef struct {
    int (*open)(const char *path, uint8_t mode);
    int (*write)(int fd, const void *buf, int len);
    int (*read)(int fd, void *buf, int len);
    int (*fstat)(int fd, struct os_fs_stat_buf *buf);
    int (*stat)(const char *path, struct os_fs_stat_buf *buf);
    int (*close)(int fd);
    int (*mkdir)(const char *path);
    int (*opendir)(const char *path);
    int (*readdir)(int fd, struct os_fs_dirent *dirent);
    int (*closedir)(int fd);
    int (*remove)(const char *path);
    int (*chdir)(const char *path);
    char *(*getcwd)(char *path, int size);
} ftpd_callback_t;

void ftpd_run(uint16_t port, const ftpd_callback_t *callback);
