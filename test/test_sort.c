#include "bscl_sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int compare(void*v1,void *v2){
    int *a1=v1;
    int *a2=v2;
    if(*a1<*a2)
    {
        return -1;
    }else if(*a1==*a2){
        return 0;
    }else{
        return 1;
    }
}

void *at(void*container,int n){
    int *a=container;
    return a+n;
}
#define LEN  30
int main(void){

  int a[LEN]={9,8,7,6,5,4,3,2,1,0};
  srand(time(NULL));
  for (int i = 0; i < LEN; ++i) {
      a[i] = rand();
  }
  for (int i = 0; i < sizeof(a) / sizeof(int); ++i) {
    printf("%d ", a[i]);
  }
  printf("\n");
  const bscl_sort_container_if_t int_if = {.size = sizeof(int), .compare = compare, .at = at};
  bscl_qsort(a, 0, LEN - 1, &int_if, NULL);
    for(int i=0;i<sizeof(a)/sizeof(int);++i){
        printf("%d ",a[i]);
    }
    printf("\n");
    return 0;
}
