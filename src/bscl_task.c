#include "bscl_task.h"
#include "bscl_common.h"
#include "bscl_list_head.h"
#include <time.h>
#include "bscl_os.h"
 struct bscl_task_t_{
bscl_list_head_t head;
bscl_task_routine_t routine;
jmp_buf jumper;
   bscl_scheduler_t *scheduler;
};
 struct bscl_scheduler_t_{
  jmp_buf jumper;
  bscl_list_head_t tasks_list;
};

void bscl_scheduler_start(bscl_scheduler_t *scheduler){
 
  while(1){
    bscl_task_t *task;
    bscl_list_for_each_entry(task,&scheduler->tasks_list,bscl_task_t,head)
      {
        int ret=setjmp(scheduler->jumper);
        if(ret==0){
          longjmp(task->jumper,1);
        }
      }
    bscl_os_sleep(1);
  }
}

bscl_scheduler_t* bscl_scheduler_new(void){
  bscl_scheduler_t *scheduler=(bscl_scheduler_t *)malloc(sizeof(bscl_scheduler_t));
  bscl_list_init(&scheduler->tasks_list); 
  return scheduler;
}

bscl_task_t* bscl_task_new(bscl_task_routine_t routine){
  bscl_task_t *task=(bscl_task_t *)malloc(sizeof(bscl_task_t ));
  if(!task)
    return NULL;
  task->routine=routine;
   
  return task;
}

int bscl_task_start(bscl_scheduler_t *scheduler,bscl_task_t*task,void*arg){
  bscl_list_insert_tail(&scheduler->tasks_list,&task->head) ;
  task->scheduler=scheduler;
  int ret=setjmp(task->jumper);
  if(ret==0){return 0;}
  else{
   ret= task->routine(task,arg);
   bscl_list_head_remove(&task->head);
   longjmp(scheduler->jumper,ret);
  }
  return 0;
}

int bscl_task_yeild(bscl_task_t*task){
  int ret=setjmp(task->jumper); 
  if(ret==0)
    {
      longjmp(task->scheduler->jumper,0);

    }
  return 0; 
}
void bscl_task_delete(bscl_task_t *task);


