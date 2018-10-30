#pragma once
#include <windows.h>

#define platform_os_sleep(t) Sleep(t*1000)
#define platform_os_sleep_ms(t) Sleep(t)

#if defined(__GNUC__)
#include <time.h>
#define _TIMESPEC_DEFINED
#include <pthread.h>
#include <semaphore.h>
typedef pthread_t platform_os_tid_t;

struct platform_os_task_attr {
    void *stack_area;
    size_t stack_size;
    int priority;
};


/*task*/
typedef void *(*platform_os_task_routine_t)(void *);

#define platform_os_task_create(tid, attr, routine, arg)                         \
    pthread_create(tid, NULL, routine, arg)
#define platform_os_task_self() pthread_self()
#define platform_os_task_join(tid, retval) pthread_join(tid, retval)


/*mutex*/
typedef pthread_mutex_t platform_os_mutex_t;

#define platform_os_mutex_create(mutex) pthread_mutex_init(mutex, NULL)
#define platform_os_mutex_lock(mutex) pthread_mutex_lock(mutex)
#define platform_os_mutex_unlock(mutex) pthread_mutex_unlock(mutex)
#define platform_os_mutex_trylock(mutex) pthread_mutex_trylock(mutex)
static inline int platform_os_mutex_timedlock(platform_os_mutex_t *mutex, unsigned int timeout_ms)
{
    struct timespec ts;
    ts.tv_sec = timeout_ms / 1000;
    ts.tv_nsec = timeout_ms % 1000 * 1000000;
    return pthread_mutex_timedlock(mutex, &ts);
}

#define platform_os_mutex_delete(mutex) pthread_mutex_destroy(mutex)

/*sem*/
typedef sem_t platform_os_sem_t;

#define platform_os_sem_create(sem,value) sem_init(sem,0,value)
#define platform_os_sem_post(sem) sem_post(sem)
#define platform_os_sem_wait(sem)  sem_wait(sem)
#define platform_os_sem_trywait(sem)  sem_trywait(sem)
#define platform_os_sem_timedwait(sem,timeout)
#define platform_os_sem_delete(sem)

#elif defined(_MSC_VER)

#include <thr/xthreads.h>

typedef _Thrd_t platform_os_tid_t;
typedef _Thrd_start_t platform_os_task_routine_t;

#define platform_os_task_create(tid, attr, routine, arg)                         \
    _Thrd_create(tid, (_Thrd_start_t)routine, arg)
#define platform_os_task_self() _Thrd_current()
#define platform_os_task_join(tid, retval) _Thrd_join(tid, retval)

typedef _Mtx_t platform_os_mutex_t;
#define platform_os_mutex_create(mutex, cname)                                          \
    _Mtx_init(mutex, _Mtx_timed )
#define platform_os_mutex_lock(mutex) _Mtx_lock(*mutex)
#define platform_os_mutex_unlock(mutex) _Mtx_unlock(*mutex)
static inline int platform_os_mutex_timedlock(platform_os_mutex_t *mutex, unsigned int timeout_ms)
{
    struct xtime ts;
    ts.sec = timeout_ms / 1000;
    ts.nsec = timeout_ms % 1000 * 1000000;
    return _Mtx_timedlock(*mutex, &ts);
}
#define platform_os_mutex_delete(mutex) _Mtx_destroy(*mutex)

/*sem*/
typedef struct platform_os_sem_win32_t_* platform_os_sem_t;

int platform_os_sem_create(platform_os_sem_t *sem, const char cname[4], unsigned int value);
int platform_os_sem_post(platform_os_sem_t *sem);
int platform_os_sem_post_isr(platform_os_sem_t *sem);
int platform_os_sem_flush(platform_os_sem_t *sem);
int platform_os_sem_wait(platform_os_sem_t *sem);
int platform_os_sem_trywait(platform_os_sem_t *sem);
int platform_os_sem_timedwait(platform_os_sem_t *sem, unsigned int timeout_ms);
void platform_os_sem_delete(platform_os_sem_t *sem);

#else 
#error not supported compiler
#endif
