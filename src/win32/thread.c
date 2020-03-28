#include <windows.h>
#include "bscl.h"

struct bscl_thread_win32_t_ {
  HANDLE handle;
  void *arg;
  bscl_thread_routine_t routine;
};

unsigned long thread_proc(void *arg) {
  struct bscl_thread_win32_t_ *tsk = (struct bscl_thread_win32_t_ *)arg;
  tsk->routine(tsk->arg);
  return 0;
}

int bscl_thread_create(bscl_thread_t *tid, struct bscl_thread_attr *attr, bscl_thread_routine_t routine, void *arg) {
  *tid = malloc(sizeof(struct bscl_thread_win32_t_));
  if (!(*tid)) {
    return -1;
  }
  (*tid)->routine = routine;
  (*tid)->handle = CreateThread(NULL, 0, thread_proc, *tid, 0, NULL);
  (*tid)->arg = arg;
  if ((*tid)->handle == INVALID_HANDLE_VALUE) {
    return -1;
  }
  return 0;
}

bscl_thread_t bscl_thread_self(void) {
  return NULL;
}

int bscl_thread_join(bscl_thread_t tid, void **retval) {
  return 0;
}
