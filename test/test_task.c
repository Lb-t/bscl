#include "platform.h"
#include "platform_task.h"
int r1(platform_task_t *task,void*arg)
{
  int cnt=0;
  printf("hello,%d\n",cnt);
  platform_task_yeild(task);
  return 0;
}

int r2(platform_task_t *task,void*arg)
{
int cnt=0;
  printf("hi,%d\n",cnt);
  platform_task_yeild(task);

  return 0;
}

int main(int argc,char*argv[])
{
platform_scheduler_t*scheduler= platform_scheduler_new();
   char a[]="hello";
  platform_task_t *task1=platform_task_new(r1);
  platform_task_start(scheduler,task1,a);
  platform_task_t *task2=platform_task_new(r2);
  platform_task_start(scheduler,task2,a);

  platform_scheduler_start(scheduler);
  return 0;
}
