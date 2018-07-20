#ifndef LIST_HEAD_H_
#define LIST_HEAD_H_

typedef struct list_head_t_ {
    struct list_head_t_ *prev, *next;
} list_head_t;

#define list_head_init(name)                                                   \
    {                                                                          \
        &(name), &(name)                                                       \
    }

#define list_head_define(name) struct list_head name = list_head_init(name)

#define list_head_data(s, mem, head) (s *)((char *)(head)-offsetof(s, mem))
#define list_head_next(head) ((head)->next)
#define list_head_prev(head) ((head)->prev)
#define list_head_insert_next(head, newHead)                                   \
    do {                                                                       \
        if ((head) != (newHead)) {                                             \
            (newHead)->prev = head;                                            \
            (newHead)->next = (head)->next;                                    \
            (head)->next = newHead;                                            \
            if ((newHead)->next)                                               \
                (newHead)->next->prev = newHead;                               \
        }                                                                      \
    } while (0)
#define list_head_insert_prev(head, newHead)                                   \
    do {                                                                       \
        if ((head) != (newHead)) {                                             \
            (newHead)->prev = (head)->prev;                                    \
            (newHead)->next = head;                                            \
            (head)->prev = newHead;                                            \
            if ((newHead)->prev)                                               \
                (newHead)->prev->next = newHead;                               \
        }                                                                      \
    } while (0)
#define list_head_remove(head)                                                 \
    do {                                                                       \
        if ((head)->prev)                                                      \
            (head)->prev->next = (head)->next;                                 \
        if ((head)->next)                                                      \
            (head)->next->prev = (head)->prev;                                 \
    } while (0)

#define list_is_last(list, head) ((list) == (head)->next)
#define list_is_empty(list) ((list)->next == (list))

#define list_insert_tail(list, head) list_head_insert_prev(list, head)

#define list_first_entry(ptr, type, member)                                    \
    list_head_data(type, member, (ptr)->next)

#define list_for_each(pos, list)                                               \
    for (pos = (list)->next; pos != (list); pos = pos->next)

#define list_for_each_prev(pos, list)                                          \
    for (pos = (list)->prev; pos != (list); pos = pos->prev)

#define list_for_each_safe(pos, n, list)                                       \
    for (pos = (list)->next, n = pos->next; pos != (list);                     \
         pos = n, n = pos->next)

#define list_for_each_prev_safe(pos, n, list)                                  \
    for (pos = (list)->prev, n = pos->prev; pos != (list);                     \
         pos = n, n = pos->prev)

#define list_for_each_entry(pos, list, type, member)                           \
    for ((pos) = list_head_data(type, member, (list)->next);                   \
         &(pos)->member != (list);                                             \
         (pos) = list_head_data(type, member, (pos)->member.next))

#define list_for_each_entry_reverse(pos, list, type, member)                   \
    for (pos = list_head_data(type, member, (list)->prev);                     \
         &pos->member != (list);                                               \
         pos = list_head_data(type, member, pos->member.prev))

#define list_init(list)                                                        \
    do {                                                                       \
        (list)->prev = list;                                                   \
        (list)->next = list;                                                   \
    } while (0)
#endif