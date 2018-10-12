#pragma once


typedef struct platform_task_t_ platform_task_t;
typedef int (*platform_task_routine_t)(platform_task_t*task, void*arg);
typedef struct platform_scheduler_t_ platform_scheduler_t;

platform_task_t* platform_task_new(platform_task_routine_t routine);
void platform_task_delete(platform_task_t *task);
int platform_task_start(platform_scheduler_t*scheduler, platform_task_t *task,void*arg);
int platform_task_stop(platform_task_t *task);
int platform_task_yeild(platform_task_t *task);
void platform_scheduler_start(platform_scheduler_t *scheduler);
platform_scheduler_t* platform_scheduler_new(void);


