#pragma once
#include <windows.h>

#define bscl_os_sleep(t) Sleep(t*1000)
#define bscl_os_sleep_ms(t) Sleep(t)
void os_usleep(long long usec);

#include <thr/xthreads.h>

typedef _Thrd_t bscl_os_tid_t;
typedef _Thrd_start_t bscl_os_task_routine_t;

#define bscl_os_task_create(tid, attr, routine, arg)                         \
    _Thrd_create(tid, (_Thrd_start_t)routine, arg)
#define bscl_os_task_self() _Thrd_current()
#define bscl_os_task_join(tid, retval) _Thrd_join(tid, retval)

typedef _Mtx_t bscl_os_mutex_t;
#define bscl_os_mutex_create(mutex, cname)                                          \
    _Mtx_init(mutex, _Mtx_timed )
#define bscl_os_mutex_lock(mutex) _Mtx_lock(*mutex)
#define bscl_os_mutex_unlock(mutex) _Mtx_unlock(*mutex)
static inline int bscl_os_mutex_timedlock(bscl_os_mutex_t *mutex, unsigned int timeout_ms)
{
    struct xtime ts;
    ts.sec = timeout_ms / 1000;
    ts.nsec = timeout_ms % 1000 * 1000000;
    return _Mtx_timedlock(*mutex, &ts);
}
#define bscl_os_mutex_delete(mutex) _Mtx_destroy(*mutex)

/*sem*/
typedef struct bscl_os_sem_win32_t_* bscl_os_sem_t;

int bscl_os_sem_create(bscl_os_sem_t *sem, const char cname[4], unsigned int value);
int bscl_os_sem_post(bscl_os_sem_t *sem);
int bscl_os_sem_post_isr(bscl_os_sem_t *sem);
int bscl_os_sem_flush(bscl_os_sem_t *sem);
int bscl_os_sem_wait(bscl_os_sem_t *sem);
int bscl_os_sem_trywait(bscl_os_sem_t *sem);
int bscl_os_sem_timedwait(bscl_os_sem_t *sem, unsigned int timeout_ms);
void bscl_os_sem_delete(bscl_os_sem_t *sem);


