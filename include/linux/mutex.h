#pragma once

#if defined(__STDC_NO_THREADS__) || defined(__GNUC__)
#include <time.h>
#define _TIMESPEC_DEFINED
#include <pthread.h>

typedef pthread_mutex_t bscl_os_mutex_t;


#define bscl_os_mutex_create(mutex, cname) pthread_mutex_init(mutex, NULL)

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
#elif defined(_MSC_VER)

#endif



