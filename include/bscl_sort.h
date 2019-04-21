#pragma once 
#include <stddef.h>
typedef int (*compare_fun_t)(void*v1,void*v2);
typedef void* (*at_fun_t)(void*container,int n);//random access function
typedef void (*set_fun_t)(void*container,int n,void*value);
typedef struct{
compare_fun_t compare;
size_t size;//size of a member
at_fun_t at;
set_fun_t set;
}bscl_sort_container_if_t;

void bscl_qsort(void*container,int first,int last,const bscl_sort_container_if_t *container_if,void*temp);
