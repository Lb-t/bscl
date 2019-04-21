#pragma once
#include "bscl_common.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct bscl_list_t_ bscl_list_t;
typedef struct bscl_list_item_t_ bscl_list_item_t;

bscl_list_t *bscl_list_new(void);
bool bscl_list_insert(bscl_list_t *list, bscl_list_item_t *item, void *data, size_t len);
size_t bscl_list_item_data(bscl_list_item_t *item, void **data);
bscl_list_item_t *bscl_list_head(bscl_list_t *list);
bscl_list_item_t *bscl_list_tail(bscl_list_t *list);
size_t bscl_list_size(bscl_list_t *list);
bool bscl_list_remove(bscl_list_t *list, bscl_list_item_t *item);
void bscl_list_delete(bscl_list_t *list);
bscl_list_item_t *bscl_list_item_prev(bscl_list_item_t *item);
bscl_list_item_t *bscl_list_item_next(bscl_list_item_t *item);
