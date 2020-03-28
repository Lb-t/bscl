#include "bscl_tasklet.h"
#include "bscl_os.h"
#include <stdlib.h>
#include <string.h>
void bscl_tasklet_init(struct bscl_tasklet *tsl) {
  memset(tsl, 0, sizeof(*tsl));
  bscl_list_init(&tsl->list);
}

void bscl_tasklet_init2(struct bscl_tasklet *tsl, bscl_tasklet_routine_t routine, void *arg) {
  bscl_tasklet_init(tsl);
  tsl->run = routine;
  tsl->arg = arg;
}

void bscl_tasklet_head_init(struct bscl_tasklet_head *th, bscl_sem_t *sem, bscl_mutex_t *mutex) {
  memset(th, 0, sizeof(*th));
  th->sem = sem;
  th->mutex = mutex;
  bscl_list_init(&th->list);
}

int bscl_tasklet_link(struct bscl_tasklet *tsl, struct bscl_tasklet_head *th) {
  if (!bscl_list_is_empty(&tsl->list))
    return -1;

  if (th->mutex) {
    bscl_mutex_lock(th->mutex);
  }

  bscl_list_insert_tail(&tsl->list, &th->list);

  if (th->mutex) {
    bscl_mutex_unlock(th->mutex);
  }
  tsl->head = th;

  return 0;
}

int bscl_tasklet_is_linked(struct bscl_tasklet *tsl) { return bscl_list_is_empty(&tsl->list) ? 0 : 1; }

void bscl_tasklet_unlink(struct bscl_tasklet *tsl) {
  struct bscl_tasklet_head *th = tsl->head;

  if (bscl_list_is_empty(&tsl->list))
    return;

  if (th->mutex) {
    bscl_mutex_lock(th->mutex);
  }

  bscl_list_head_remove(&tsl->list);

  if (th->mutex) {
    bscl_mutex_unlock(th->mutex);
  }
}

void bscl_tasklet_pend(struct bscl_tasklet *tsl) {
  if (tsl->pend == tsl->deal) {
    tsl->pend++;
    if (tsl->head->sem) {
      bscl_sem_post(tsl->head->sem);
    }
  } else {
    tsl->pend++;
  }
}

void bscl_tasklet_pend_isr(struct bscl_tasklet *tsl) { bscl_tasklet_pend(tsl); }

int bscl_tasklet_is_pended(struct bscl_tasklet *tsl) { return tsl->pend != tsl->deal ? 1 : 0; }

void bscl_tasklet_clean_pend(struct bscl_tasklet *tsl) { tsl->deal = tsl->pend; }

int bscl_tasklet_dispatch(struct bscl_tasklet_head *th, int timeout_ms) {
  int count = 0;
  struct bscl_tasklet *tsl, *temp;

  tsl = NULL;

  if (timeout_ms > 0 && th->sem != NULL) {
    bscl_sem_timedwait(th->sem, timeout_ms);
  }

  if (th->mutex) {
    bscl_mutex_lock(th->mutex);
  }

  bscl_list_for_each_entry_safe(tsl, temp, &th->list, struct bscl_tasklet, list) {
    if (bscl_tasklet_is_pended(tsl)) {
      bscl_tasklet_clean_pend(tsl);
      if (tsl->run != NULL)
        tsl->run(tsl);
      count++;
    }
  }

  if (th->mutex) {
    bscl_mutex_unlock(th->mutex);
  }

  return count;
}
