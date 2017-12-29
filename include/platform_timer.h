
#ifndef _INCLUDE_PLATFORM_TIMER_H_
#define _INCLUDE_PLATFORM_TIMER_H_
#include <stdint.h>
#ifndef _WIN32
#include "container/platform_list_head.h"
#endif
typedef struct {
#ifdef _WIN32
  unsigned int id;
#else
  platform_list_head_t list_head;
  unsigned int ms;
#endif
  void (*callback)(void *);
  void *data;
} platform_timer_t;

platform_timer_t *platform_timer_new(void (*callback)(void *), uint16_t ms,
                                     void *data);
void platform_timer_delete(platform_timer_t *timer);
#endif /*!_INCLUDE_PLATFORM_TIMER_H_*/
