#pragma once

#include <time.h>
#include <sys/time.h>
#include <semaphore.h>
typedef sem_t bscl_os_sem_t;
#define bscl_os_sem_create(sem, cname, value) sem_init(sem, 0, value)
#define bscl_os_sem_post(sem) sem_post(sem)
#define bscl_os_sem_post_isr(sem) sem_post(sem)
#define bscl_os_sem_wait(sem) sem_wait(sem)
#define bscl_os_sem_trywait(sem) sem_trywait(sem)
#define bscl_os_sem_delete(sem) sem_destroy(sem)
static inline int bscl_os_sem_timedwait(bscl_os_sem_t *sem, unsigned int timeout_ms) {
  return -1;
}
