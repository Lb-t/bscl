#pragma once

#pragma once

typedef struct bscl_os_mutex_win32_t_* bscl_os_mutex_t;

int bscl_os_mutex_create(bscl_os_mutex_t *mutex);
int bscl_os_mutex_lock(bscl_os_mutex_t *mutex);
int bscl_os_mutex_unlock(bscl_os_mutex_t *mutex);
int bscl_os_mutex_trylock(bscl_os_mutex_t *mutex);
int bscl_os_mutex_timedlock(bscl_os_mutex_t *mutex, int timeout_ms);
void bscl_os_mutex_delete(bscl_os_mutex_t *mutex);



