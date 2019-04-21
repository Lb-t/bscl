#include "bscl.h"
#include "bscl_task.h"
#include <stdio.h>
int r1(bscl_task_t *task,void*arg)
{
  int cnt=0;
  printf("hello,%d\n",cnt);
  bscl_task_yeild(task);
  return 0;
}

int r2(bscl_task_t *task,void*arg)
{
int cnt=0;
  printf("hi,%d\n",cnt);
  bscl_task_yeild(task);

  return 0;
}

int main(int argc,char*argv[])
{
bscl_scheduler_t*scheduler= bscl_scheduler_new();
   char a[]="hello";
  bscl_task_t *task1=bscl_task_new(r1);
  bscl_task_start(scheduler,task1,a);
  bscl_task_t *task2=bscl_task_new(r2);
  bscl_task_start(scheduler,task2,a);

  bscl_scheduler_start(scheduler);
  return 0;
}
