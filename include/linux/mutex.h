#pragma once

#include <time.h>
#include <pthread.h>

typedef pthread_mutex_t bscl_mutex_t;

#define bscl_mutex_create(mutex) pthread_mutex_init(mutex, NULL)

#define bscl_mutex_lock(mutex) pthread_mutex_lock(mutex)

#define bscl_mutex_unlock(mutex) pthread_mutex_unlock(mutex)

#define bscl_mutex_trylock(mutex) pthread_mutex_trylock(mutex)

static inline int bscl_mutex_timedlock(bscl_mutex_t *mutex, unsigned int timeout_ms) {
  return -1;
}

#define bscl_mutex_delete(mutex) pthread_mutex_destroy(mutex)
