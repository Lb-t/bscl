#pragma once


typedef struct bscl_task_t_ bscl_task_t;
typedef int (*bscl_task_routine_t)(bscl_task_t*task, void*arg);
typedef struct bscl_scheduler_t_ bscl_scheduler_t;

bscl_task_t* bscl_task_new(bscl_task_routine_t routine);
void bscl_task_delete(bscl_task_t *task);
int bscl_task_start(bscl_scheduler_t*scheduler, bscl_task_t *task,void*arg);
int bscl_task_stop(bscl_task_t *task);
int bscl_task_yeild(bscl_task_t *task);
void bscl_scheduler_start(bscl_scheduler_t *scheduler);
bscl_scheduler_t* bscl_scheduler_new(void);


