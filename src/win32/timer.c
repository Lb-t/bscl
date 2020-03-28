#include <windows.h>
#include "bscl.h"

struct bscl_timer_t_ {
  unsigned int handle;
  int ticks;
  void *arg;
  bscl_timer_routine_t routine;
};

int bscl_timer_create(bscl_timer_t *tim) {
  memset(tim, 0, sizeof(*tim));
  return 0;
}

void bscl_timer_cancel(bscl_timer_t *tim) {
  if (tim->handle)
    timeKillEvent(tim->handle);
}

void bscl_timer_delete(bscl_timer_t *tim) {
  if (tim->handle)
    timeKillEvent(tim->handle);
}

static void CALLBACK __os_timer_svc(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
  bscl_timer_t *tim = (bscl_timer_t *)dwUser;

  if (tim->routine) {
    tim->routine(tim, tim->arg);
  }
}

int bscl_timer_fire(bscl_timer_t *tim, int ticks, bscl_timer_routine_t routine, void *arg) {
  tim->routine = routine;
  tim->arg = arg;
  tim->ticks = ticks;
  if (ticks > 0) {
    tim->handle = timeSetEvent(abs(ticks), 0, __os_timer_svc, (DWORD_PTR)tim, TIME_ONESHOT | TIME_KILL_SYNCHRONOUS);
  } else {
    tim->handle = timeSetEvent(abs(ticks), 0, __os_timer_svc, (DWORD_PTR)tim, TIME_PERIODIC | TIME_KILL_SYNCHRONOUS);
  }

  if (!tim->handle) {
    return -1;
  }

  return 0;
}
