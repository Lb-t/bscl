/*
 * list.h
 *
 *  Created on: 2016骞�6鏈�29鏃�
 *      Author: qh
 */

#ifndef _INCLUDE_PLATFORM_LIST_H_
#define _INCLUDE_PLATFORM_LIST_H_
#include "common/platform_common.h"
#include "stdbool.h"
#include "stdint.h"

typedef struct platform_list_t_ platform_list_t;
typedef struct platform_list_item_t_ platform_list_item_t;

platform_list_t *platform_list_new(void);
bool platform_list_insert(platform_list_t *list, platform_list_item_t *item,
                          void *data, size_t len);
size_t platform_list_item_data(platform_list_item_t *item, void **data);
platform_list_item_t *platform_list_head(platform_list_t *list);
platform_list_item_t *platform_list_tail(platform_list_t *list);
size_t platform_list_size(platform_list_t *list);
bool platform_list_remove(platform_list_t *list, platform_list_item_t *item);
void platform_list_delete(platform_list_t *list);
platform_list_item_t *platform_list_item_prev(platform_list_item_t *item);
platform_list_item_t *platform_list_item_next(platform_list_item_t *item);
#endif /* !_INCLUDE_PLATFORM_LIST_H_ */
