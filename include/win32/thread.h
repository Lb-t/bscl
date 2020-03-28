#pragma once
#include <stddef.h>
typedef void *(*bscl_thread_routine_t)(void *);
struct bscl_thread_attr {
  void *stack_area;
  size_t stack_size;
  int priority;
};

typedef struct bscl_thread_win32_t_ *bscl_thread_t;

int bscl_thread_create(bscl_thread_t *tid, struct bscl_thread_attr *attr, bscl_thread_routine_t routine, void *arg);
bscl_thread_t bscl_thread_self(void);
int bscl_thread_join(bscl_thread_t tid, void **retval);

