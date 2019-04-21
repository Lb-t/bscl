#pragma once
#include "bscl_list_head.h"
#include "bscl_os.h"
#include <stdint.h>
struct bscl_tasklet_head;
struct bscl_tasklet;

typedef int (*bscl_tasklet_routine_t)(struct bscl_tasklet *tsl);

struct bscl_tasklet {
  bscl_tasklet_routine_t run;
  void *arg;

  bscl_list_head_t list;
  void *head_data;
  struct bscl_tasklet_head *head;

  volatile uint32_t pend;
  volatile uint32_t deal;
};
void bscl_tasklet_init(struct bscl_tasklet *tsl);
void bscl_tasklet_init2(struct bscl_tasklet *tsl, bscl_tasklet_routine_t routine, void *arg);

int bscl_tasklet_link(struct bscl_tasklet *tsl, struct bscl_tasklet_head *th);
int bscl_tasklet_is_linked(struct bscl_tasklet *tsl);
void bscl_tasklet_unlink(struct bscl_tasklet *tsl);

void bscl_tasklet_pend(struct bscl_tasklet *tsl);
void bscl_tasklet_pend_isr(struct bscl_tasklet *tsl);
int bscl_tasklet_is_pended(struct bscl_tasklet *tsl);
void bscl_tasklet_clean_pend(struct bscl_tasklet *tsl);

struct bscl_tasklet_head {
  bscl_list_head_t list;
  void *arg;

  bscl_os_sem_t *sem;
  bscl_os_mutex_t *mutex;
};

void bscl_tasklet_head_init(struct bscl_tasklet_head *th, bscl_os_sem_t *sem, bscl_os_mutex_t *mutex);

int bscl_tasklet_dispatch(struct bscl_tasklet_head *th, int timeout_ms);
