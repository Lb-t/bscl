#pragma once
#include "platform_list_head.h"
#include "platform_os.h"
#include <stdint.h>
struct platform_tasklet_head;
struct platform_tasklet;

typedef int (*platform_tasklet_routine_t)(struct platform_tasklet *tsl);

struct platform_tasklet {
  platform_tasklet_routine_t run;
  void *arg;

  platform_list_head_t list;
  void *head_data;
  struct platform_tasklet_head *head;

  volatile uint32_t pend;
  volatile uint32_t deal;
};
void platform_tasklet_init(struct platform_tasklet *tsl);
void platform_tasklet_init2(struct platform_tasklet *tsl, platform_tasklet_routine_t routine, void *arg);

int platform_tasklet_link(struct platform_tasklet *tsl, struct platform_tasklet_head *th);
int platform_tasklet_is_linked(struct platform_tasklet *tsl);
void platform_tasklet_unlink(struct platform_tasklet *tsl);

void platform_tasklet_pend(struct platform_tasklet *tsl);
void platform_tasklet_pend_isr(struct platform_tasklet *tsl);
int platform_tasklet_is_pended(struct platform_tasklet *tsl);
void platform_tasklet_clean_pend(struct platform_tasklet *tsl);

struct platform_tasklet_head {
  platform_list_head_t list;
  void *arg;

  platform_os_sem_t *sem;
  platform_os_mutex_t *mutex;
};

void platform_tasklet_head_init(struct platform_tasklet_head *th, platform_os_sem_t *sem, platform_os_mutex_t *mutex);

int platform_tasklet_dispatch(struct platform_tasklet_head *th, int timeout_ms);
