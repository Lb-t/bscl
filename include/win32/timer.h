#pragma once
typedef struct {
  unsigned int id;
  void (*callback)(void *);
  void *data;
} bscl_timer_t;

