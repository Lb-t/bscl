/*
 * Bin's utility library
 */
#ifndef _INCLUDE_PLATFORM_TIMER_H_
#define _INCLUDE_PLATFORM_TIMER_H_
#include <stdint.h>

typedef struct {
  void (*callback)(void *);
  void *data;
  unsigned int id;
} platform_timer_t;

platform_timer_t *platform_timer_new(void (*callback)(void *), uint16_t ms,
                                     void *data);
void platform_timer_delete(platform_timer_t *timer);
#endif /*!_INCLUDE_PLATFORM_TIMER_H_*/
