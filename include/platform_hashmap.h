#ifndef INCLUDE_PLATFORM_HASHMAP_H_
#define INCLUDE_PLATFORM_HASHMAP_H_
#include "platform_list_head.h"

typedef struct {
  size_t key_size;
  platform_list_head_t **buck;
  size_t buck_len;
  size_t (*hash)(void *);
} platform_hashmap_t;

platform_hashmap_t *platform_hashmap_new(size_t key_size, 
                                         size_t buck_len, size_t hash(void *));
void platform_hashmap_set(platform_hashmap_t *this, void *key, void *value ,size_t value_size);
void const *platform_hashmap_get(platform_hashmap_t *this, void *key);
void platform_hashmap_remove(platform_hashmap_t *this, void *key);
void platform_hashmap_delete(platform_hashmap_t *this);
#endif