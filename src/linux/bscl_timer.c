#include "bscl_os.h"
#include "bscl_common.h"
#include "bscl_list_head.h"

#include <signal.h>
#include <stdio.h>
#include <sys/time.h>
struct bscl_timer_t_ {
  bscl_list_head_t list_head;
  unsigned int ms;
  void (*callback)(void *);
  void *data;
};

static bscl_timer_t *bscl_timer_start = NULL;
void bscl_timer_on(int arg) {
  bscl_timer_t *timer;

  static unsigned int count = 0;
  timer = bscl_timer_start;
  while (timer) {
    if (count % timer->ms == (timer->ms - 1)) {
      timer->callback(timer->data);
    }
    bscl_list_head_t *h = bscl_list_head_next(&timer->list_head);
    if (h) {
      timer = bscl_list_head_data(bscl_timer_t, list_head, h);
    } else {
      timer = NULL;
    }
  }
  count++;
}

bscl_timer_t *bscl_timer_new(void (*callback)(void *), uint16_t ms, void *data) {
  bscl_assert(callback);
  bscl_assert(ms);
  bscl_timer_t *timer = (bscl_timer_t *)malloc(sizeof(bscl_timer_t));
  timer->callback = callback;
  timer->data = data;

  timer->ms = ms;
  if (!bscl_timer_start) {

    struct itimerval tick;
    signal(SIGALRM, bscl_timer_on);
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
    bscl_timer_start = timer;
  } else {
    bscl_list_head_insert_prev((bscl_list_head_t *)bscl_timer_start, (bscl_list_head_t *)timer);
  }
  return timer;
}

void bscl_timer_delete(bscl_timer_t *this) {
  bscl_timer_t *timer = bscl_timer_start;
  bscl_list_head_t *h;
  while (timer) {
    if (timer == this) {
      if (bscl_timer_start == this) {
        h = bscl_list_head_next(&this->list_head);
        if (h) {
          bscl_timer_start = bscl_list_head_data(bscl_timer_t, list_head, h);
        } else {
          bscl_timer_start = NULL;
        }
      }
      bscl_list_head_remove(&this->list_head);
      break;
    }
    h = bscl_list_head_next(&timer->list_head);
    if (h) {
      timer = bscl_list_head_data(bscl_timer_t, list_head, h);
    }
  }
  if (!bscl_timer_start) {
    struct itimerval tick;
    tick.it_value.tv_sec = 0;  // sec
    tick.it_value.tv_usec = 0; // micro sec.
                               // Interval time to run function
    tick.it_interval.tv_sec = 0;
    tick.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &tick, NULL);
  }
  free(this);
}
