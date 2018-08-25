#ifndef LIST_HEAD_H_
#define LIST_HEAD_H_

typedef struct platform_list_head_t_ {
    struct platform_list_head_t_ *prev, *next;
} platform_list_head_t;

#define platform_list_head_init(name)                                                   \
    {                                                                          \
        &(name), &(name)                                                       \
    }


#define platform_list_head_define(name) struct platform_list_head name = platform_list_head_init(name)


#define platform_list_head_data(s, mem, head) (s *)((char *)(head)-offsetof(s, mem))
#define platform_list_head_next(head) ((head)->next)
#define platform_list_head_prev(head) ((head)->prev)
#define platform_list_head_insert_next(head, newHead)                                   \
    do {                                                                       \
        if ((head) != (newHead)) {                                             \
            (newHead)->prev = head;                                            \
            (newHead)->next = (head)->next;                                    \
            (head)->next = newHead;                                            \
            if ((newHead)->next)                                               \
                (newHead)->next->prev = newHead;                               \
        }                                                                      \
    } while (0)
#define platform_list_head_insert_prev(head, newHead)                                   \
    do {                                                                       \
        if ((head) != (newHead)) {                                             \
            (newHead)->prev = (head)->prev;                                    \
            (newHead)->next = head;                                            \
            (head)->prev = newHead;                                            \
            if ((newHead)->prev)                                               \
                (newHead)->prev->next = newHead;                               \
        }                                                                      \
    } while (0)
#define platform_list_head_remove(head)                                                 \
    do {                                                                       \
        if ((head)->prev)                                                      \
            (head)->prev->next = (head)->next;                                 \
        if ((head)->next)                                                      \
            (head)->next->prev = (head)->prev;                                 \
    } while (0)

#define platform_listis_last(list, head) ((list) == (head)->next)
#define platform_listis_empty(list) ((list)->next == (list))

#define platform_list_insert_tail(list, head) platform_list_head_insert_prev(list, head)

#define platform_list_first_entry(ptr, type, member)                                    \
    platform_list_head_data(type, member, (ptr)->next)

#define platform_listfor_each(pos, list)                                               \
    for (pos = (list)->next; pos != (list); pos = pos->next)

#define platform_listfor_each_prev(pos, list)                                          \
    for (pos = (list)->prev; pos != (list); pos = pos->prev)

#define platform_listfor_each_safe(pos, n, list)                                       \
    for (pos = (list)->next, n = pos->next; pos != (list);                     \
         pos = n, n = pos->next)

#define platform_listfor_each_prev_safe(pos, n, list)                                  \
    for (pos = (list)->prev, n = pos->prev; pos != (list);                     \
         pos = n, n = pos->prev)


#define platform_listfor_each_entry(pos, list, type, member)                           \
    for ((pos) = platform_list_head_data(type, member, (list)->next);                   \
         &(pos)->member != (list);                                             \
         (pos) = platform_list_head_data(type, member, (pos)->member.next))


#define platform_listfor_each_entry_reverse(pos, list, type, member)                   \
    for (pos = platform_list_head_data(type, member, (list)->prev);                     \
         &pos->member != (list);                                               \
         pos = platform_list_head_data(type, member, pos->member.prev))

#define platform_listinit(list)                                                        \
    do {                                                                       \
        (list)->prev = list;                                                   \
        (list)->next = list;                                                   \
    } while (0)
#endif
