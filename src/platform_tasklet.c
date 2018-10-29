#include "platform_tasklet.h"
#include <stdlib.h>
#include <string.h>
#include "platform_os.h"
void platform_tasklet_init(struct platform_tasklet *tsl)
{
    memset(tsl, 0,sizeof(*tsl));
    list_init(&tsl->list);
}

void platform_tasklet_init2(struct platform_tasklet *tsl, platform_tasklet_routine_t routine, void *arg)
{
    platform_tasklet_init(tsl);
    tsl->run = routine;
    tsl->arg = arg;
}

void platform_tasklet_head_init(struct platform_tasklet_head *th, platform_sem_t *sem, platform_mutex_t *mutex)
{
    memset(th, 0,sizeof(*th));
    th->sem = sem;
    th->mutex = mutex;
    list_init(&th->list);
}

int platform_tasklet_link(struct platform_tasklet *tsl, struct platform_tasklet_head *th)
{
    if (!list_is_empty(&tsl->list))
        return -1;

    if(th->mutex) {
        platform_os_mutex_lock(th->mutex);
    }

    list_insert_tail(&tsl->list, &th->list);

    if(th->mutex) {
        platform_os_mutex_unlock(th->mutex);
    }
    tsl->head = th;

    return 0;
}

int platform_tasklet_is_linked(struct platform_tasklet *tsl)
{
    return list_is_empty(&tsl->list) ? 0 : 1;
}

void platform_tasklet_unlink(struct platform_tasklet *tsl)
{
    struct platform_tasklet_head *th = tsl->head;

    if(list_is_empty(&tsl->list)) return;

    if(th->mutex) {
        platform_os_mutex_lock(th->mutex);
    }

    list_head_remove(&tsl->list);

    if(th->mutex) {
        platform_os_mutex_unlock(th->mutex);
    }
}

void platform_tasklet_pend(struct platform_tasklet *tsl)
{
    if(tsl->pend ==tsl->deal) {
        tsl->pend++;
        if(tsl->head->sem) {
            platform__os_sem_post(tsl->head->sem);
        }
    } else {
        tsl->pend++;
    }
}

void platform_tasklet_pend_isr(struct platform_tasklet *tsl)
{
    platform_tasklet_pend(tsl);
}

int platform_tasklet_is_pended(struct platform_tasklet *tsl)
{
    return tsl->pend != tsl->deal ? 1 : 0;
}

void platform_tasklet_clean_pend(struct platform_tasklet *tsl)
{
    tsl->deal = tsl->pend;
}

int platform_tasklet_dispatch(struct platform_tasklet_head *th, int timeout_ms)
{
    int count = 0;
    struct platform_tasklet *tsl, *temp;

    tsl = NULL;

    if(timeout_ms >0 && th->sem !=NULL) {
        platform_os_sem_timedwait(th->sem, timeout_ms);
    }

    if(th->mutex) {
        platform_os_mutex_lock(th->mutex);
    }

    list_for_each_entry_safe(tsl, temp, &th->list, struct platform_tasklet,list)
    {
        if(platform_tasklet_is_pended(tsl)) {
            platform_tasklet_clean_pend(tsl);
            if(tsl->run !=NULL) tsl->run(tsl);
            count++;
        }
    }

    if(th->mutex) {
        platform_os_mutex_unlock(th->mutex);
    }

    return count;
}

