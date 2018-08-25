#pragma once


typedef int (*platform_task_routine_t)(void*arg);
typedef struct platform_task_t_ platform_task_t;
typedef struct platform_scheduler_t_ platform_scheduler_t;
platform_task_t* platform_task_new(platform_task_routine_t routine);
void platform_task_delete(platform_task_t *task);
int platform_task_start(platform_task_t *task);
int platform_task_stop(platform_task_t *task);
int platform_task_yeild(platform_task_t *task);


