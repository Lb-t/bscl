#pragma once
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#define bscl_os_sleep(t) sleep(t)
#define bscl_os_sleep_ms(t) usleep(t*1000)

typedef pthread_t bscl_os_tid_t;

struct bscl_os_task_attr {
    void *stack_area;
    size_t stack_size;
    int priority;
};

typedef void *(*bscl_os_task_routine_t)(void *);

#define bscl_os_task_create(tid, attr, routine, arg)                         \
    pthread_create(tid, NULL, routine, arg)
#define bscl_os_task_self() pthread_self()
#define bscl_os_task_join(tid, retval) pthread_join(tid, retval)


typedef pthread_mutex_t bscl_os_mutex_t;

#define bscl_os_mutex_create(mutex) pthread_mutex_init(mutex, NULL)
#define bscl_os_mutex_lock(mutex) pthread_mutex_lock(mutex)
#define bscl_os_mutex_unlock(mutex) pthread_mutex_unlock(mutex)
#define bscl_os_mutex_trylock(mutex) pthread_mutex_trylock(mutex)

static inline int bscl_os_mutex_timedlock(bscl_os_mutex_t *mutex, unsigned int timeout_ms)
{
  return -1;
}

#define bscl_os_mutex_delete(mutex) pthread_mutex_destroy(mutex)

/*sem*/
typedef sem_t bscl_os_sem_t;

#define bscl_os_sem_create(sem,value) sem_init(sem,0,value)
#define bscl_os_sem_post(sem) sem_post(sem)
#define bscl_os_sem_wait(sem)  sem_wait(sem)
#define bscl_os_sem_trywait(sem)  sem_trywait(sem)
#define bscl_os_sem_timedwait(sem,timeout)
#define bscl_os_sem_delete(sem)



