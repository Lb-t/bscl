#pragma once
#include "bscl_list_head.h"
typedef struct {
  bscl_list_head_t list_head;
  unsigned int ms;
  void (*callback)(void *);
  void *data;
} bscl_timer_t;
