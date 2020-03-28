#include "bscl_fs.h"
#include <windows.h>
#include <stdbool.h>

#define MAX_OPEN_FILE 64

typedef struct {
  HANDLE h;
  uint8_t data[0];
} bscl_file_win32_t;

static bscl_file_win32_t *files[MAX_OPEN_FILE];

static int new_fd(uint8_t type) {
  size_t size = sizeof(bscl_file_win32_t);

  if (type == BSCL_FS_TYPE_DIR) {
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

static void delete_fd(int fd) {
  free(files[fd]);
  files[fd] = NULL;
}

int bscl_fs_open(const char *path, uint8_t mode) {
  DWORD dwDesiredAccess = 0;
  DWORD dwCreationDisposition = 0;
  int fd = new_fd(BSCL_FS_TYPE_REG);
  if (fd < 0) {
    return -1;
  }

  if ((mode & BSCL_FS_MODE_CREATE) && (mode & BSCL_FS_MODE_TRUNC)) {
    dwCreationDisposition |= CREATE_ALWAYS;
  } else if (mode & BSCL_FS_MODE_CREATE) {
    dwCreationDisposition |= OPEN_ALWAYS;
  } else if (mode & BSCL_FS_MODE_TRUNC) {
    dwCreationDisposition |= TRUNCATE_EXISTING;
  } else {
    dwCreationDisposition |= OPEN_EXISTING;
  }

  if (mode & BSCL_FS_MODE_READ) {
    dwDesiredAccess |= GENERIC_READ;
  }
  if (mode & BSCL_FS_MODE_WRITE) {
    dwDesiredAccess |= GENERIC_WRITE;
  }

  bscl_file_win32_t *f = files[fd];
  f->h = CreateFile(path, dwDesiredAccess, 0, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
  if (f->h == INVALID_HANDLE_VALUE) {
    delete_fd(fd);
    return -1;
  }

  return fd;
}

int bscl_fs_write(int fd, const void *buf, int len) {
  bscl_file_win32_t *f = files[fd];
  DWORD written;
  if (!WriteFile(f->h, buf, len, &written, NULL)) {
    return -1;
  }
  return written;
}

int bscl_fs_read(int fd, void *buf, int len) {
  bscl_file_win32_t *f = files[fd];
  DWORD read;
  if (!ReadFile(f->h, buf, len, &read, NULL)) {
    return -1;
  }
  return read;
}

int bscl_fs_seek(int fd, int offset, uint8_t origin) {
  bscl_file_win32_t *f = files[fd];
  DWORD dwMoveMethod;
  if (origin == BSCL_FS_SEEK_SET) {
    dwMoveMethod = FILE_BEGIN;
  } else if (origin == BSCL_FS_SEEK_CUR) {
    dwMoveMethod = FILE_CURRENT;
  } else if (origin == BSCL_FS_SEEK_END) {
    dwMoveMethod = FILE_END;
  } else {
    return -1;
  }

  SetFilePointer(f->h, offset, NULL, dwMoveMethod);
  return 0;
}

int bscl_fs_tell(int fd) {
  bscl_file_win32_t *f = files[fd];
  return SetFilePointer(f->h, 0, NULL, FILE_CURRENT);
}

static time_t convertWindowsTimeToUnixTime(FILETIME *ftime) {
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

int bscl_fs_fstat(int fd, struct bscl_fs_stat_buf *buf) {
  bscl_file_win32_t *f = files[fd];
  BY_HANDLE_FILE_INFORMATION info;
  if (!GetFileInformationByHandle(f->h, &info)) {
    return -1;
  }
  buf->size = info.nFileSizeLow;
  buf->time = convertWindowsTimeToUnixTime(&info.ftLastWriteTime);

  if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
    buf->type = BSCL_FS_TYPE_DIR;
  } else {
    buf->type = BSCL_FS_TYPE_REG;
  }
  return 0;
}

int bscl_fs_stat(const char *path, struct bscl_fs_stat_buf *buf) {
  WIN32_FILE_ATTRIBUTE_DATA info;
  if (!GetFileAttributesEx(path, GetFileExInfoStandard, &info)) {
    return -1;
  }
  buf->size = info.nFileSizeLow;
  buf->time = convertWindowsTimeToUnixTime(&info.ftLastWriteTime);
  if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
    buf->type = BSCL_FS_TYPE_DIR;
  } else {
    buf->type = BSCL_FS_TYPE_REG;
  }

  return 0;
}

int bscl_fs_flush(int fd) {
  bscl_file_win32_t *f = files[fd];
  FlushFileBuffers(f->h);
  return 0;
}

int bscl_fs_close(int fd) {
  CloseHandle(files[fd]->h);
  delete_fd(fd);
  return 0;
}

int bscl_fs_mkdir(const char *path) {
  return CreateDirectory(path, 0);
}

int bscl_fs_opendir(const char *path) {
  const char str_to_append[] = "\\*";
  int path_len = strlen(path);
  char *find_str = malloc(path_len + sizeof(str_to_append));

  if (!find_str) {
    return -1;
  }

  memcpy(find_str, path, path_len);
  memcpy(find_str + path_len, str_to_append, sizeof(str_to_append));

  int fd = new_fd(BSCL_FS_TYPE_DIR);
  if (fd < 0) {
    return -1;
  }
  bscl_file_win32_t *f = files[fd];
  f->h = FindFirstFile(find_str, (WIN32_FIND_DATA *)(f->data + 1));
  free(find_str);
  if (INVALID_HANDLE_VALUE == f->h) {
    delete_fd(fd);
    return -1;
  }

  f->data[0] = true;
  return 0;
}

int bscl_fs_readdir(int fd, struct bscl_fs_dirent *dirent) {
  bscl_file_win32_t *f = files[fd];
  WIN32_FIND_DATA *data = (WIN32_FIND_DATA *)(f->data + 1);
  bool valid = f->data[0];
  if (!valid) {
    return -1;
  }

  strncpy(dirent->name, data->cFileName, BSCL_FS_PATH_MAX);
  if (data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
    dirent->type = BSCL_FS_TYPE_DIR;
  } else {
    dirent->type = BSCL_FS_TYPE_REG;
  }

  if (!FindNextFile(f->h, data)) {
    f->data[0] = false;
  }
  return 0;
}

int bscl_fs_closedir(int fd) {
  bscl_file_win32_t *f = files[fd];
  FindClose(f->h);
  delete_fd(fd);
  return 0;
}

int bscl_fs_remove(const char *path) {
  if (bscl_fs_isdir(path)) {
    return RemoveDirectory(path);
  } else {
    return DeleteFile(path);
  }
}

bool bscl_fs_exists(const char *path) {
  return GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES;
}

bool bscl_fs_isdir(const char *path) {
  DWORD x = GetFileAttributes(path);
  return x != INVALID_FILE_ATTRIBUTES && (x & FILE_ATTRIBUTE_DIRECTORY);
}

int64 bscl_fs_mtime(const char *path) {
  WIN32_FILE_ATTRIBUTE_DATA info;
  BOOL r = GetFileAttributesEx(path, GetFileExInfoStandard, &info);
  if (!r)
    return -1;

  FILETIME &wt = info.ftLastWriteTime;
  return ((int64)wt.dwHighDateTime << 32) | wt.dwLowDateTime;
}

int64 bscl_fs_fsize(const char *path) {
  WIN32_FILE_ATTRIBUTE_DATA info;
  BOOL r = GetFileAttributesEx(path, GetFileExInfoStandard, &info);
  if (!r)
    return -1;
  return ((int64)info.nFileSizeHigh << 32) | info.nFileSizeLow;
}

bool bscl_fs_rename(const char *from, const char *to) {
  return MoveFile(from, to);
}

bool bscl_fs_symlink(const char *dst, const char *lnk) {
  return CreateSymbolicLink(lnk, dst, bscl_fs_isdir(dst));
}
