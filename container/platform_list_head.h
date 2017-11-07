#ifndef INCLUDE_PLATFORM_LIST_HEAD_H_
#define INCLUDE_PLATFORM_LIST_HEAD_H_
#include "common/platform_common.h"

typedef struct platform_list_head_t_ {
  struct platform_list_head_t_ *prev, *next;
} platform_list_head_t;

#define platform_list_head_data(s, mem, head)                                  \
  (s *)((char *)(head)-offsetof(s, mem))
#define platform_list_head_next(head) ((head)->next)
#define platform_list_head_prev(head) ((head)->prev)
#define platform_list_head_insert_next(head, newHead)                          \
  do {                                                                         \
    \
if(head != newHead) {                                                          \
      \
(newHead)->prev = head;                                                        \
      \
(newHead)->next = (head)->next;                                                \
      \
(head)->next = newHead;                                                        \
      \
if((newHead->next))\
(newHead)->next->prev = newHead;                                               \
    }                                                                          \
  } while (0)
#define platform_list_head_insert_prev(head, newHead)                          \
  do {                                                                         \
    \
if(head != newHead) {                                                          \
      (newHead)->prev = (head)->prev;                                          \
      (newHead)->next = head;                                                  \
      (head)->prev = newHead;                                                  \
      if ((newHead)->prev)                                                     \
        (newHead)->prev->next = newHead;                                       \
    }                                                                          \
  } while (0)
#define platform_list_head_remove(head)                                        \
  do {                                                                         \
    if (head->prev)                                                            \
      head->prev->next = head->next;                                           \
    if (head->next)                                                            \
      head->next->prev = head->prev;                                           \
  } while (0)
#endif