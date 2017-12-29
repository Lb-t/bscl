#ifndef INCLUDE_PLATFORM_QUEUE_H_
#define INCLUDE_PLATFORM_QUEUE_H_
#include "platform_common.h"
#include "platform_list.h"
#define platform_queue_t platform_list_t
#define platform_queue_push(queue, data, len)                                  \
  platform_list_insert(queue, NULL, data, len)
#define platform_queue_pop(queue, data, len)                                   \
  do {                                                                         \
    platform_list_item_t *item = platform_list_tail(queue);                    \
    if (item) {                                                                \
      *len = platform_list_item_data(item, data);                              \
      platform_list_remove(queue, item);                                       \
    } else {                                                                   \
      *len = 0;                                                                \
      *data = NULL;                                                            \
    }                                                                          \
  } while (0)
#define platform_queue_new() platform_list_new()
#define platform_queue_delete(queue) platform_list_delete(queue)
#define platform_queue_size(queue) platform_list_size(queue)

#endif // !INCLUDE_PLATFORM_QUEUE_H_
