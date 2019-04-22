#pragma once
#include <time.h>
#define _TIMESPEC_DEFINED
#include <pthread.h>
#include <semaphore.h>

#define bscl_os_sleep(t) _sleep(t*1000)
#define bscl_os_sleep_ms(t) _sleep(t)
#define os_usleep(us)   usleep(us)

typedef pthread_t bscl_os_tid_t;

struct bscl_os_task_attr {
    void *stack_area;
    size_t stack_size;
    int priority;
};


/*task*/
typedef void *(*bscl_os_task_routine_t)(void *);

#define bscl_os_task_create(tid, attr, routine, arg)                         \
    pthread_create(tid, NULL, routine, arg)
#define bscl_os_task_self() pthread_self()
#define bscl_os_task_join(tid, retval) pthread_join(tid, retval)


/*mutex*/
typedef pthread_mutex_t bscl_os_mutex_t;

#define bscl_os_mutex_create(mutex) pthread_mutex_init(mutex, NULL)
#define bscl_os_mutex_lock(mutex) pthread_mutex_lock(mutex)
#define bscl_os_mutex_unlock(mutex) pthread_mutex_unlock(mutex)
#define bscl_os_mutex_trylock(mutex) pthread_mutex_trylock(mutex)
static inline int bscl_os_mutex_timedlock(bscl_os_mutex_t *mutex, unsigned int timeout_ms)
{
    struct timespec ts;
    ts.tv_sec = timeout_ms / 1000;
    ts.tv_nsec = timeout_ms % 1000 * 1000000;
    return pthread_mutex_timedlock(mutex, &ts);
}

#define bscl_os_mutex_delete(mutex) pthread_mutex_destroy(mutex)

/*sem*/
typedef sem_t bscl_os_sem_t;

#define bscl_os_sem_create(sem,value) sem_init(sem,0,value)
#define bscl_os_sem_post(sem) sem_post(sem)
#define bscl_os_sem_wait(sem)  sem_wait(sem)
#define bscl_os_sem_trywait(sem)  sem_trywait(sem)
#define bscl_os_sem_delete(sem) sem_destroy(sem)
#define gettimeofday(t,z) mingw_gettimeofday(t, z)

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
