#include "platform_timer.h"
#include "platform_common.h"

#ifdef _WIN32
#include <windows.h>
//#include <mmsystem.h>
//#pragma comment(lib, "Winmm.lib")
#else
#include <signal.h>
#include <stdio.h>
#include <sys/time.h>
#endif
static void (*platform_time_callback)(void *);
static void *platform_timer_callbackData = NULL;

#ifdef _WIN32
void CALLBACK platform_timer_on(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser,
                                DWORD_PTR dw1, DWORD_PTR dw2)
#else
static platform_timer_t *platform_timer_start = NULL;
void platform_timer_on(int arg)
#endif
{
  platform_timer_t *timer;
#ifdef _WIN32
  timer = (platform_timer_t *)dwUser;
  timer->callback(timer->data);
#else
  static unsigned int count = 0;
  timer = platform_timer_start;
  while (timer) {
    if (count % timer->ms == (timer->ms - 1)) {
      timer->callback(timer->data);
    }
    platform_list_head_t *h = platform_list_head_next(&timer->list_head);
    if (h) {
      timer = platform_list_head_data(platform_timer_t, list_head, h);
    } else {
      timer = NULL;
    }
  }
  count++;
#endif
}

platform_timer_t *platform_timer_new(void (*callback)(void *), uint16_t ms,
                                     void *data) {
  platform_assert(callback);
  platform_assert(ms);
  platform_timer_t *timer =
      (platform_timer_t *)malloc(sizeof(platform_timer_t));
  timer->callback = callback;
  timer->data = data;

#ifdef _WIN32
  timer->id = timeSetEvent(ms, 0, platform_timer_on, (DWORD_PTR)timer,
                           TIME_PERIODIC | TIME_KILL_SYNCHRONOUS);
#else
  timer->ms = ms;
  if (!platform_timer_start) {

    struct itimerval tick;
    signal(SIGALRM, platform_timer_on);
    tick.it_value.tv_sec = 0;     // sec
    tick.it_value.tv_usec = 1000; // micro sec.
                                  // Interval time to run function
    tick.it_interval.tv_sec = 0;
    tick.it_interval.tv_usec = 1000;
    int res = setitimer(ITIMER_REAL, &tick, NULL);
    if (res) {
      printf("Set timer failed!!\n");
      return NULL;
    }
    platform_timer_start = timer;
  } else {
    platform_list_head_insert_prev((platform_list_head_t *)platform_timer_start,
                                   (platform_list_head_t *)timer);
  }
#endif
  return timer;
}

void platform_timer_delete(platform_timer_t *this) {
#ifdef _WIN32
  timeKillEvent(this->id);
#else
  platform_timer_t *timer = platform_timer_start;
  platform_list_head_t *h;
  while (timer) {
    if (timer == this) {
      if (platform_timer_start = this) {
        h = platform_list_head_next(&this->list_head);
        if (h) {
          platform_timer_start =
              platform_list_head_data(platform_timer_t, list_head, h);
        } else {
          platform_timer_start = NULL;
        }
      }
      platform_list_head_remove(&this->list_head);
      break;
    }
    h = platform_list_head_next(&timer->list_head);
    if (h) {
      timer = platform_list_head_data(platform_timer_t, list_head, h);
    }
  }
  if (!platform_timer_start) {
    struct itimerval tick;
    tick.it_value.tv_sec = 0;  // sec
    tick.it_value.tv_usec = 0; // micro sec.
                               // Interval time to run function
    tick.it_interval.tv_sec = 0;
    tick.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &tick, NULL);
  }
#endif
  free(this);
}
