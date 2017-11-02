#include "platform_timer.h"
#include "platform_common.h"

#ifdef _WIN32
#include <windows.h>
//#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")
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
void platform_timer_on(int arg)
#endif
{
  platform_timer_t *timer;
#ifdef _WIN32
  timer = (platform_timer_t *)dwUser;
#elif
#endif
  timer->callback(timer->data);
}

platform_timer_t *platform_timer_new(void (*callback)(void *), uint16_t ms,
                                     void *data)
{
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
  struct itimerval tick;
  signal(SIGALRM, platform_timer_on);
  tick.it_value.tv_sec = 0;          // sec
  tick.it_value.tv_usec = ms * 1000; // micro sec.
                                     // Interval time to run function
  tick.it_interval.tv_sec = 0;
  tick.it_interval.tv_usec = ms * 1000;
  int res = setitimer(ITIMER_REAL, &tick, NULL);
  if (res)
  {
    printf("Set timer failed!!\n");
  }
#endif
  return timer;
}

void platform_timer_delete(platform_timer_t *this)
{
#ifdef _WIN32
  timeKillEvent(this->id);
#else
  struct itimerval tick;
  tick.it_value.tv_sec = 0;  // sec
  tick.it_value.tv_usec = 0; // micro sec.
                             // Interval time to run function
  tick.it_interval.tv_sec = 0;
  tick.it_interval.tv_usec = 0;
  setitimer(ITIMER_REAL, &tick, NULL);
#endif
  free(this);
}
