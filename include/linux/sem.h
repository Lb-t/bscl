#pragma once

#if defined(__GNUC__)
#include <time.h>
#define _TIMESPEC_DEFINED
#include <semaphore.h>
typedef sem_t os_sem_t;
#define os_sem_create(sem, cname, value) sem_init(sem, 0, value)
#define os_sem_post(sem) sem_post(sem)
#define os_sem_post_isr(sem) sem_post(sem)
#define os_sem_wait(sem) sem_wait(sem)
#define os_sem_trywait(sem) sem_trywait(sem)
#define os_sem_delete(sem) sem_destroy(sem)
#ifdef __WIN32
#define gettimeofday(t,z) mingw_gettimeofday(t, z)
#endif
static inline int os_sem_timedwait(os_sem_t *sem, unsigned int timeout_ms)
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

typedef struct os_sem_win32_t_ *os_sem_t;

int os_sem_create(os_sem_t *sem, const char cname[4], unsigned int value);
int os_sem_post(os_sem_t *sem);
int os_sem_post_isr(os_sem_t *sem);
int os_sem_flush(os_sem_t *sem);
int os_sem_wait(os_sem_t *sem);
int os_sem_trywait(os_sem_t *sem);
int os_sem_timedwait(os_sem_t *sem, unsigned int timeout_ms);
void os_sem_delete(os_sem_t *sem);
#else
#error "unknow compiler"
#endif
