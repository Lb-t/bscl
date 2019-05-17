#pragma once

#if defined(__GNUC__)
#include <time.h>
#define _TIMESPEC_DEFINED
#include <semaphore.h>
typedef sem_t bscl_os_sem_t;
#define bscl_os_sem_create(sem, cname, value) sem_init(sem, 0, value)
#define bscl_os_sem_post(sem) sem_post(sem)
#define bscl_os_sem_post_isr(sem) sem_post(sem)
#define bscl_os_sem_wait(sem) sem_wait(sem)
#define bscl_os_sem_trywait(sem) sem_trywait(sem)
#define bscl_os_sem_delete(sem) sem_destroy(sem)
#ifdef __WIN32
#define gettimeofday(t,z) mingw_gettimeofday(t, z)
#endif
static inline int bscl_os_sem_timedwait(bscl_os_sem_t *sem, unsigned int timeout_ms)
{
    struct timespec ts;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ts.tv_sec = timeout_ms / 1000 + tv.tv_sec;
    timeout_ms %= 1000;
    timeout_ms += tv.tv_usec / 1000;
    tv.tv_usec %= 1000;
    ts.tv_sec += timeout_ms / 1000;
    timeout_ms %= 1000;
    ts.tv_nsec = timeout_ms * 1000000 + tv.tv_usec * 1000;
    return sem_timedwait(sem, &ts);
}
#elif defined(_MSC_VER)

typedef struct bscl_os_sem_win32_t_ *bscl_os_sem_t;

int bscl_os_sem_create(bscl_os_sem_t *sem, const char cname[4], unsigned int value);
int bscl_os_sem_post(bscl_os_sem_t *sem);
int bscl_os_sem_post_isr(bscl_os_sem_t *sem);
int bscl_os_sem_flush(bscl_os_sem_t *sem);
int bscl_os_sem_wait(bscl_os_sem_t *sem);
int bscl_os_sem_trywait(bscl_os_sem_t *sem);
int bscl_os_sem_timedwait(bscl_os_sem_t *sem, unsigned int timeout_ms);
void bscl_os_sem_delete(bscl_os_sem_t *sem);
#else
#error "unknow compiler"
#endif
