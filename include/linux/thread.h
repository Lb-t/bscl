#pragma once
#include <stddef.h>
typedef void *(*bscl_os_thread_routine_t)(void *);
struct bscl_os_thread_attr {
    void *stack_area;
    size_t stack_size;
    int priority;
};

#include <time.h>
#define _TIMESPEC_DEFINED
#include <pthread.h>

typedef pthread_t bscl_os_tid_t;


#define bscl_os_thread_create(tid, attr, routine, arg)                         \
    pthread_create(tid, NULL, routine, arg)
#define bscl_os_thread_self() pthread_self()
#define bscl_os_thread_join(tid, retval) pthread_join(tid, retval)


