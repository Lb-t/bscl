#pragma once
#include <unistd.h>
#include <pthread.h>
#define platform_os_sleep(t) sleep(t)
#define platform_os_sleep_ms(t) usleep(t*1000)

typedef pthread_t platform_os_tid_t;

struct platform_os_task_attr {
    void *stack_area;
    size_t stack_size;
    int priority;
};

typedef void *(*platform_os_task_routine_t)(void *);

#define platform_os_task_create(tid, attr, routine, arg)                         \
    pthread_create(tid, NULL, routine, arg)
#define platform_os_task_self() pthread_self()
#define platform_os_task_join(tid, retval) pthread_join(tid, retval)


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