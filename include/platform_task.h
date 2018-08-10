#pragma once
#include "platform_queue.h"
#include <setjmp.h>


typedef int (*runnable_entry_t)(void*arg);
typedef struct platform_task_t_ platform_task_t;

platform_task_t* platform_task_new(void);
void platform_task_delete(platform_task_t *task);
int platform_task_register_runnable(platform_task_t *task,platform_runnable_t *runnable);
int platform_task_wait(platform_task_t *task);
int platform_task_unregister_runnable(platform_task_t *task,platform_runnable_t *runnable);
