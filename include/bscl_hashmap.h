#pragma once
#include "bscl_list_head.h"

#include "bscl_common.h"
typedef struct {
  size_t key_size;
  bscl_list_head_t **buck;
  size_t buck_len;
  size_t (*hash)(void *);
} bscl_hashmap_t;

bscl_hashmap_t *bscl_hashmap_new(size_t key_size, size_t buck_len, size_t hash(void *));
void bscl_hashmap_set(bscl_hashmap_t *this, void *key, void *value, size_t value_size);
void const *bscl_hashmap_get(bscl_hashmap_t *this, void *key);
void bscl_hashmap_remove(bscl_hashmap_t *this, void *key);
void bscl_hashmap_delete(bscl_hashmap_t *this);
