#include "platform_task.h"
#include "platform_common.h"
#include <pthread.h>
#include "platform_list_head.h"
#include <semaphore.h>
typedef struct{
list_head_t head;
runnable_entry_t entry;
jmp_buf jumper;
}platform_runnable_t;
 struct platform_task_t_{
  jmp_buf jumper;
  pthread_t tid;
  list_head_t runnable_list;
  sem_t sem;
};

static void*routine(void*arg){
  platform_task_t*task=arg;
 
  /*config task entry*/
  int ret=setjmp(&task->jumper);
  if(ret==0)
  {
  }
  else{
  platform_runnable_t *runnable;
  /*jump to runnable*/
  longjmp(runnable->jumper,1);
  }

}



platform_task_t* platform_task_new(void){
  platform_task_t *task=(platform_task_t *)malloc(sizeof(platform_task_t ));
  if(!task)
    return NULL;
  int ret=pthread_create(&task->tid,NULL,routine,task);
  if(ret)
  {
    free(task);
    return NULL;
  }
  list_init(&task->runnable_list);  
  sem_init(&task->sem,0,0);
  return task;
}
void platform_task_delete(platform_task_t *task);
int platform_task_register_runnable(platform_task_t *task,platform_runnable_t *runnable){
   platform_assert(task);
   platform_assert(runnable);
   list_insert_tail(&task->runnable_list,&runnable->head);  
   return 0;
}
int platform_task_wait(platform_task_t *task){
   setjmp(&task->jumper);
   return 0;
}
int platform_task_unregister_runnable(platform_task_t *task,platform_runnable_t *runnable){
 list_head_remove(&runnable->head);
 return 0;
}
