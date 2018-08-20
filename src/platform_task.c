#include "platform_task.h"
#include "platform_common.h"
#include "platform_list_head.h"
#include <time.h>

 struct platform_task_t_{
list_head_t head;
platform_task_routine_t routine;
jmp_buf jumper;
   platform_scheduler_t *scheduler;
};
 struct platform_scheduler_t_{
  jmp_buf jumper;
  list_head_t tasks_list;
};

void platform_scheduler_start(platform_scheduler_t *scheduler){
 
  while(1){
    platform_task_t *task;
    list_for_each_entry(task,&scheduler->tasks_list,platform_task_t,head)
      {
        int ret=setjmp(&scheduler->jumper);
        if(ret==0){
          longjmp(&task->jumper);
        }
      }
    sleep(1);
  }
}

platform_scheduler_t* platform_scheduler_new(void){
  platform_scheduler_t *scheduler=(platform_scheduler_t *)malloc(sizeof(platform_scheduler_t));
  list_init(&scheduler->tasks_list); 
  return scheduler;
}

platform_task_t* platform_task_new(platform_scheduler_t *scheduler){
  platform_task_t *task=(platform_task_t *)malloc(sizeof(platform_task_t ));
  if(!task)
    return NULL;
  task->scheduler=scheduler;
   
  return task;
}

int platform_task_start(platform_task_t*task,void*arg){
  list_insert_tail(&scheduler.tasks_list,&task->head) ;
  int ret=setjmp(&task->jumper);
  if(ret==0){return 0;}
  else{
   ret= task->routine(arg);
   list_head_remove(&task->head);
   longjmp(&scheduler->jumper,ret);
  }
  return 0;
}

int platform_task_yeild(platform_task_t*task){
  int ret=setjmp(&task->jumper); 
  if(ret==0)
    {
      longjmp(&task->scheduler->jumper,0);

    }
  return 0; 
}
void platform_task_delete(platform_task_t *task);


