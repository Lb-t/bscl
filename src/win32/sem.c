#include "bscl.h"

#include <stdlib.h>
#include <limits.h>
#if defined(_MSC_VER)
#include <Windows.h>
#include <stdio.h>
struct bscl_sem_win32_t_ {
    HANDLE hd;
};

int bscl_sem_create(bscl_sem_t *sem, unsigned int value)
{
    (*sem) = (struct bscl_sem_win32_t_*)malloc(sizeof(struct bscl_sem_win32_t_));
    if (*sem == NULL) {
        return -1;
    }
    (*sem) -> hd = CreateSemaphore(NULL, value, SHRT_MAX,NULL);
    if ((*sem)->hd == NULL) {
        printf("GetLastError() %d\n", GetLastError());
        free(*sem);
        return -1;
    }
    return 0;
}
int bscl_sem_post(bscl_sem_t *sem)
{
    if (ReleaseSemaphore((*sem)->hd, 1, NULL)) {
        return 0;
    } else {
        return -1;
    }
}


int bscl_sem_timedwait(bscl_sem_t *sem, unsigned int timeout_ms)
{
    DWORD dwWaitResult;
    dwWaitResult = WaitForSingleObject((*sem)->hd, // handle to semaphore
                            timeout_ms); // zero-second time-out interval

    switch (dwWaitResult) {
    // The semaphore object was signaled.
    case WAIT_OBJECT_0:
        return 0;
        break;

    // The semaphore was nonsignaled, so a time-out occurred.
    case WAIT_TIMEOUT:
        return -1;
        break;
    default:
        return -1;
        break;
    }
    return -1;
}

int bscl_sem_wait(bscl_sem_t *sem)
{
    return bscl_sem_timedwait(sem,INFINITE);
}
int bscl_sem_trywait(bscl_sem_t *sem)
{
    return bscl_sem_timedwait(sem,0);
}

void bscl_sem_delete(bscl_sem_t *sem)
{
    CloseHandle((*sem)->hd);
    free(*sem);
}

#endif
