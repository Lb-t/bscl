#include "platform_sort.h"
#include<stdlib.h>
#include<time.h>
#include<assert.h>
#include<string.h>
void platform_sort_init(void){
    srand(time(NULL));
}
void platform_qsort(void*container,int first,int last,const platform_sort_container_if_t *container_if,void*temp){
    assert(first<=last);
  if (first == last)
    return;
   int i=first;
   int j=last;
   int m=i+rand()%(last-first+1);
   void*mid_value=container_if->at(container,m);
   void *value_i;
   void *value_m;
   void *value_j;
   
   void *value_temp;
   if(temp){value_temp=temp;}else{
   value_temp=malloc(container_if->size);
   }
   while(i<j){     
       value_m=container_if->at(container,m);
       while(i<m){
           value_i=container_if->at(container,i);
           int ret=container_if->compare(value_i,value_m);
           if(ret>0){
             memcpy(value_temp, value_i, container_if->size);
             memcpy(value_i, value_m, container_if->size);
             memcpy(value_m, value_temp, container_if->size);
              m=i;
              value_m=container_if->at(container,m);
           }else{
               ++i;
           }
       }

        while(m<j){
           value_j=container_if->at(container,j);
           int ret=container_if->compare(value_m,value_j);
           if(ret>0){
             memcpy(value_temp, value_j, container_if->size);
             memcpy(value_j, value_m, container_if->size);
             memcpy(value_m, value_temp, container_if->size);
              m=j;
             value_m = container_if->at(container, m);
           }else{
               --j;
           }
       }
   }
   if (first < m) {
   platform_qsort(container,first,m-1,container_if,value_temp);
   }
   if (m < last) {
   platform_qsort(container,m+1,last,container_if,value_temp);
   }
   if(!temp){
       free(value_temp);
   }
}