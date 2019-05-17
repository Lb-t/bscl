#pragma once
#include <stddef.h>
typedef void *(*bscl_os_thread_routine_t)(void *);
struct bscl_os_thread_attr {
  void *stack_area;
  size_t stack_size;
  int priority;
};

#if defined(__GNUC__)
#include <time.h>
#define _TIMESPEC_DEFINED
#include <pthread.h>

typedef pthread_t bscl_os_thread_t;

#define bscl_os_thread_create(tid, attr, routine, arg) pthread_create(tid, NULL, routine, arg)

#define bscl_os_thread_self() pthread_self()
#define bscl_os_thread_join(tid, retval) pthread_join(tid, retval)
#elif defined(_MSC_VER)

typedef struct bscl_os_thread_win32_t_ *bscl_os_thread_t;

int bscl_os_thread_create(bscl_os_thread_t *tid, struct bscl_os_thread_attr *attr, bscl_os_thread_routine_t routine, void *arg);
bscl_os_tid_t bscl_os_thread_self(void);
int bscl_os_thread_join(bscl_os_thread_t tid, void **retval);

#endif