#pragma once
#include <stddef.h>
typedef struct bscl_list_head_t_ {
  struct bscl_list_head_t_ *prev, *next;
} bscl_list_head_t;

#define bscl_list_head_init(name)                                                                                          \
  { &(name), &(name) }

#define bscl_list_head_define(name) struct bscl_list_head name = bscl_list_head_init(name)

#define bscl_list_head_data(s, mem, head) (s *)((char *)(head)-offsetof(s, mem))
#define bscl_list_head_next(head) ((head)->next)
#define bscl_list_head_prev(head) ((head)->prev)
#define bscl_list_head_insert_next(head, newHead)                                                                          \
  do {                                                                                                                         \
    if ((head) != (newHead)) {                                                                                                 \
      (newHead)->prev = head;                                                                                                  \
      (newHead)->next = (head)->next;                                                                                          \
      (head)->next = newHead;                                                                                                  \
      if ((newHead)->next)                                                                                                     \
        (newHead)->next->prev = newHead;                                                                                       \
    }                                                                                                                          \
  } while (0)
#define bscl_list_head_insert_prev(head, newHead)                                                                          \
  do {                                                                                                                         \
    if ((head) != (newHead)) {                                                                                                 \
      (newHead)->prev = (head)->prev;                                                                                          \
      (newHead)->next = head;                                                                                                  \
      (head)->prev = newHead;                                                                                                  \
      if ((newHead)->prev)                                                                                                     \
        (newHead)->prev->next = newHead;                                                                                       \
    }                                                                                                                          \
  } while (0)
#define bscl_list_head_remove(head)                                                                                        \
  do {                                                                                                                         \
    if ((head)->prev)                                                                                                          \
      (head)->prev->next = (head)->next;                                                                                       \
    if ((head)->next)                                                                                                          \
      (head)->next->prev = (head)->prev;                                                                                       \
  } while (0)

#define bscl_list_is_last(list, head) ((list) == (head)->next)
#define bscl_list_is_empty(list) ((list)->next == (list))

#define bscl_list_insert_tail(list, head) bscl_list_head_insert_prev(list, head)

#define bscl_list_first_entry(ptr, type, member) bscl_list_head_data(type, member, (ptr)->next)

#define bscl_list_for_each(pos, list) for (pos = (list)->next; pos != (list); pos = pos->next)

#define bscl_list_for_each_prev(pos, list) for (pos = (list)->prev; pos != (list); pos = pos->prev)

#define bscl_list_for_each_safe(pos, n, list) for (pos = (list)->next, n = pos->next; pos != (list); pos = n, n = pos->next)

#define bscl_list_for_each_prev_safe(pos, n, list)                                                                         \
  for (pos = (list)->prev, n = pos->prev; pos != (list); pos = n, n = pos->prev)

#define bscl_list_for_each_entry(pos, list, type, member)                                                                  \
  for ((pos) = bscl_list_head_data(type, member, (list)->next); &(pos)->member != (list);                                  \
       (pos) = bscl_list_head_data(type, member, (pos)->member.next))

#define bscl_list_for_each_entry_safe(pos, n, list, type, member)                                                          \
  for (pos = bscl_list_head_data(type, member, (list)->next), n = bscl_list_head_data(type, member, pos->member.next); \
       &pos->member != (list); pos = n, n = bscl_list_head_data(type, member, n->member.next))

#define bscl_list_for_each_entry_reverse(pos, list, type, member)                                                          \
  for (pos = bscl_list_head_data(type, member, (list)->prev); &pos->member != (list);                                      \
       pos = bscl_list_head_data(type, member, pos->member.prev))

#define bscl_list_init(list)                                                                                               \
  do {                                                                                                                         \
    (list)->prev = list;                                                                                                       \
    (list)->next = list;                                                                                                       \
  } while (0)

