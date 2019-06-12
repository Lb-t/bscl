#pragma once

#if defined(_MSC_VER) || defined(__GNUC__)

typedef struct bscl_os_sem_win32_t_ *bscl_os_sem_t;

int bscl_os_sem_create(bscl_os_sem_t *sem, unsigned int value);
int bscl_os_sem_post(bscl_os_sem_t *sem);
int bscl_os_sem_post_isr(bscl_os_sem_t *sem);
int bscl_os_sem_flush(bscl_os_sem_t *sem);
int bscl_os_sem_wait(bscl_os_sem_t *sem);
int bscl_os_sem_trywait(bscl_os_sem_t *sem);
int bscl_os_sem_timedwait(bscl_os_sem_t *sem, unsigned int timeout_ms);
void bscl_os_sem_delete(bscl_os_sem_t *sem);
#else
#error "unknow compiler"
#endif
