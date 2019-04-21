#pragma once
#include <stdint.h>
#ifndef _WIN32
#include "bscl_list_head.h"
#endif
typedef struct {
#ifdef _WIN32
  unsigned int id;
#else
  bscl_list_head_t list_head;
  unsigned int ms;
#endif
  void (*callback)(void *);
  void *data;
} bscl_timer_t;

bscl_timer_t *bscl_timer_new(void (*callback)(void *), uint16_t ms,
                                     void *data);
void bscl_timer_delete(bscl_timer_t *timer);
