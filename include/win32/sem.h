#pragma once

#if defined(_MSC_VER) || defined(__GNUC__)

typedef struct bscl_sem_win32_t_ *bscl_sem_t;

int bscl_sem_create(bscl_sem_t *sem, unsigned int value);
int bscl_sem_post(bscl_sem_t *sem);
int bscl_sem_post_isr(bscl_sem_t *sem);
int bscl_sem_flush(bscl_sem_t *sem);
int bscl_sem_wait(bscl_sem_t *sem);
int bscl_sem_trywait(bscl_sem_t *sem);
int bscl_sem_timedwait(bscl_sem_t *sem, unsigned int timeout_ms);
void bscl_sem_delete(bscl_sem_t *sem);
#else
#error "unknow compiler"
#endif
