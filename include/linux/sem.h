#pragma once

#include <time.h>
#include <sys/time.h>
#include <semaphore.h>
typedef sem_t bscl_sem_t;
#define bscl_sem_create(sem, cname, value) sem_init(sem, 0, value)
#define bscl_sem_post(sem) sem_post(sem)
#define bscl_sem_post_isr(sem) sem_post(sem)
#define bscl_sem_wait(sem) sem_wait(sem)
#define bscl_sem_trywait(sem) sem_trywait(sem)
#define bscl_sem_delete(sem) sem_destroy(sem)
static inline int bscl_sem_timedwait(bscl_sem_t *sem, unsigned int timeout_ms) {
  return -1;
}
