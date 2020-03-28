#pragma once
#include <stddef.h>
typedef void *(*bscl_thread_routine_t)(void *);
struct bscl_thread_attr {
  void *stack_area;
  size_t stack_size;
  int priority;
};

#include <time.h>
#include <pthread.h>

typedef pthread_t bscl_thread_t;

#define bscl_thread_create(tid, attr, routine, arg) pthread_create(tid, NULL, routine, arg)
#define bscl_thread_self() pthread_self()
#define bscl_thread_join(tid, retval) pthread_join(tid, retval)
