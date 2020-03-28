#include "bscl.h"
#include <windows.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
struct bscl_mutex_win32_t_ {
  HANDLE hd;
};

int os_mutex_create(bscl_mutex_t *mtx) {
  (*mtx) = (struct bscl_mutex_win32_t_ *)malloc(sizeof(struct bscl_mutex_win32_t_));
  if (*mtx == NULL) {
    return -1;
  }
  (*mtx)->hd = CreateMutex(NULL, FALSE, NULL);
  if ((*mtx)->hd == NULL) {
    free(*mtx);
    return -1;
  }
  return 0;
}
int os_mutex_unlock(bscl_mutex_t *mtx) {
  if (ReleaseMutex((*mtx)->hd)) {
    return 0;
  } else {
    return -1;
  }
}

int os_mutex_timedlock(bscl_mutex_t *mtx, int timeout_ms) {
  DWORD dwWaitResult;
  dwWaitResult = WaitForSingleObject((*mtx)->hd,  // handle to mtxaphore
                                     timeout_ms); // zero-second time-out interval

  switch (dwWaitResult) {
  // The mutex object was signaled.
  case WAIT_OBJECT_0:
    return 0;
    break;

  // The mutex was nonsignaled, so a time-out occurred.
  case WAIT_TIMEOUT:
    return -1;
    break;
  default:
    return -1;
    break;
  }
  return -1;
}

int os_mutex_lock(bscl_mutex_t *mtx) {
  return os_mutex_timedlock(mtx, INFINITE);
}
int os_mutex_trylock(bscl_mutex_t *mtx) {
  return os_mutex_timedlock(mtx, 0);
}

void os_mutex_delete(bscl_mutex_t *mtx) {
  CloseHandle((*mtx)->hd);
  free(*mtx);
}
