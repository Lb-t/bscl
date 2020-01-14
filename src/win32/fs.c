#include "win32/fs.h"
#include <windows.h>
#include <stdbool.h>

#define MAX_OPEN_FILE 64

typedef struct {
    HANDLE h;
    uint8_t data[0];
} os_file_win32_t;

static os_file_win32_t *files[MAX_OPEN_FILE];

static int new_fd(uint8_t type)
{
    size_t size = sizeof(os_file_win32_t);

    if (type == OS_FS_TYPE_DIR) {
        size += sizeof(WIN32_FIND_DATA) + 1;
    }

    for (int i = 0; i < MAX_OPEN_FILE; ++i) {
        files[i] = malloc(size);
        if (files[i]) {
            return i;
        } else {
            break;
        }
    }
    return -1;
}

static void delete_fd(int fd)
{
    free(files[fd]);
    files[fd] = NULL;
}

int os_fs_open(const char *path, uint8_t mode)
{
    DWORD dwDesiredAccess = 0;
    DWORD dwCreationDisposition = 0;
    int fd = new_fd(OS_FS_TYPE_REG);
    if (fd < 0) {
        return -1;
    }

    if ((mode & OS_FS_MODE_CREATE) && (mode & OS_FS_MODE_TRUNC)) {
        dwCreationDisposition |= CREATE_ALWAYS;
    } else if (mode & OS_FS_MODE_CREATE) {
        dwCreationDisposition |= OPEN_ALWAYS;
    } else if (mode & OS_FS_MODE_TRUNC) {
        dwCreationDisposition |= TRUNCATE_EXISTING;
    } else {
        dwCreationDisposition |= OPEN_EXISTING;
    }

    if (mode & OS_FS_MODE_READ) {
        dwDesiredAccess |= GENERIC_READ;
    }
    if (mode & OS_FS_MODE_WRITE) {
        dwDesiredAccess |= GENERIC_WRITE;
    }

    os_file_win32_t *f = files[fd];
    f->h = CreateFile(path, dwDesiredAccess, 0, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL,
                      NULL);
    if (f->h == INVALID_HANDLE_VALUE) {
        delete_fd(fd);
        return -1;
    }

    return fd;
}

int os_fs_write(int fd, const void *buf, int len)
{
    os_file_win32_t *f = files[fd];
    DWORD written;
    if (!WriteFile(f->h, buf, len, &written, NULL)) {
        return -1;
    }
    return written;
}

int os_fs_read(int fd, void *buf, int len)
{
    os_file_win32_t *f = files[fd];
    DWORD read;
    if (!ReadFile(f->h, buf, len, &read, NULL)) {
        return -1;
    }
    return read;
}

int os_fs_seek(int fd, int offset, uint8_t origin)
{
    os_file_win32_t *f = files[fd];
    DWORD dwMoveMethod;
    if (origin == OS_FS_SEEK_SET) {
        dwMoveMethod = FILE_BEGIN;
    } else if (origin == OS_FS_SEEK_CUR) {
        dwMoveMethod = FILE_CURRENT;
    } else if (origin == OS_FS_SEEK_END) {
        dwMoveMethod = FILE_END;
    } else {
        return -1;
    }

    SetFilePointer(f->h, offset, NULL, dwMoveMethod);
    return 0;
}

int os_fs_tell(int fd)
{
    os_file_win32_t *f = files[fd];
    return SetFilePointer(f->h, 0, NULL, FILE_CURRENT);
}

static time_t convertWindowsTimeToUnixTime(FILETIME *ftime)
{
    const uint64_t TICKS_PER_SECOND = 10000000;
    const uint64_t EPOCH_DIFFERENCE = 11644473600;
    union {
        uint64_t tick;
        FILETIME ftime;
    } input;

    memcpy(&input, ftime, sizeof(*ftime));
    uint64_t temp;
    temp = input.tick / TICKS_PER_SECOND; // convert from 100ns intervals to seconds;
    temp = temp - EPOCH_DIFFERENCE;       // subtract number of seconds between epochs
    return (time_t)temp;
}

int os_fs_fstat(int fd, struct os_fs_stat_buf *buf)
{
    os_file_win32_t *f = files[fd];
    BY_HANDLE_FILE_INFORMATION info;
    if (!GetFileInformationByHandle(f->h, &info)) {
        return -1;
    }
    buf->size = info.nFileSizeLow;
    buf->time = convertWindowsTimeToUnixTime(&info.ftLastWriteTime);

    if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        buf->type = OS_FS_TYPE_DIR;
    } else {
        buf->type = OS_FS_TYPE_REG;
    }
    return 0;
}

int os_fs_stat(const char *path, struct os_fs_stat_buf *buf)
{
    WIN32_FILE_ATTRIBUTE_DATA info;
    if (!GetFileAttributesEx(path, GetFileExInfoStandard, &info)) {
        return -1;
    }
    buf->size = info.nFileSizeLow;
    buf->time = convertWindowsTimeToUnixTime(&info.ftLastWriteTime);
    if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        buf->type = OS_FS_TYPE_DIR;
    } else {
        buf->type = OS_FS_TYPE_REG;
    }

    return 0;
}

int os_fs_flush(int fd)
{
    os_file_win32_t *f = files[fd];
    FlushFileBuffers(f->h);
    return 0;
}

int os_fs_close(int fd)
{
    CloseHandle(files[fd]->h);
    delete_fd(fd);
    return 0;
}

int os_fs_mkdir(const char *path)
{
    return 0;
}

int os_fs_opendir(const char *path)
{
    const char str_to_append[] = "\\*";
    int path_len = strlen(path);
    char *find_str = malloc(path_len + sizeof(str_to_append));

    if (!find_str) {
        return -1;
    }

    memcpy(find_str, path, path_len);
    memcpy(find_str + path_len, str_to_append, sizeof(str_to_append));

    int fd = new_fd(OS_FS_TYPE_DIR);
    if (fd < 0) {
        return -1;
    }
    os_file_win32_t *f = files[fd];
    f->h = FindFirstFile(find_str, (WIN32_FIND_DATA *)(f->data + 1));
    free(find_str);
    if (INVALID_HANDLE_VALUE == f->h) {
        delete_fd(fd);
        return -1;
    }

    f->data[0] = true;
    return 0;
}

int os_fs_readdir(int fd, struct os_fs_dirent *dirent)
{
    os_file_win32_t *f = files[fd];
    WIN32_FIND_DATA *data = (WIN32_FIND_DATA *)(f->data + 1);
    bool valid = f->data[0];
    if (!valid) {
        return -1;
    }

    strncpy(dirent->name, data->cFileName, OS_FS_PATH_MAX);
    if (data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        dirent->type = OS_FS_TYPE_DIR;
    } else {
        dirent->type = OS_FS_TYPE_REG;
    }

    if (!FindNextFile(f->h, data)) {
        f->data[0] = false;
    }
    return 0;
}

int os_fs_closedir(int fd)
{
    os_file_win32_t *f = files[fd];
    FindClose(f->h);
    delete_fd(fd);
    return 0;
}

int os_fs_remove(const char *path)
{
    DeleteFile(path);
    return 0;
}
