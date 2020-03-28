#pragma once

typedef struct bscl_timer_t_ bscl_timer_t;
typedef void (*bscl_timer_routine_t)(bscl_timer_t *tim, void *arg);
struct __os_timer_s {
  int ticks; // <0 periodical timer, >0 oneshot timer
  void *arg;
  bscl_timer_routine_t routine;
  unsigned int handle;
};

int os_timer_create(bscl_timer_t *tim);
void os_timer_cancel(bscl_timer_t *tim);
void os_timer_delete(bscl_timer_t *tim);

int __os_timer_fire(bscl_timer_t *tim, int ticks, bscl_timer_routine_t routine, void *arg);
#define os_timer_fire_after(tim, ticks, routine, arg) __os_timer_fire(tim, ticks, routine, arg)
#define os_timer_fire_auto(tim, ticks, routine, arg) __os_timer_fire(tim, -1 * abs(ticks), routine, arg)
