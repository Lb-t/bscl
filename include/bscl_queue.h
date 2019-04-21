#pragma once
#include "bscl_common.h"
#include "bscl_list.h"
#define bscl_queue_t bscl_list_t
#define bscl_queue_push(queue, data, len)                                  \
  bscl_list_insert(queue, NULL, data, len)
#define bscl_queue_pop(queue, data, len)                                   \
  do {                                                                         \
    bscl_list_item_t *item = bscl_list_tail(queue);                    \
    if (item) {                                                                \
      *len = bscl_list_item_data(item, data);                              \
      bscl_list_remove(queue, item);                                       \
    } else {                                                                   \
      *len = 0;                                                                \
      *data = NULL;                                                            \
    }                                                                          \
  } while (0)
#define bscl_queue_new() bscl_list_new()
#define bscl_queue_delete(queue) bscl_list_delete(queue)
#define bscl_queue_size(queue) bscl_list_size(queue)

