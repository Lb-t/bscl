#include "platform_hashmap.h"
#include <stdlib.h>
#include <stdint.h>
size_t platform_hashmap_defalthash(void *key) { return 0; }

platform_hashmap_t *platform_hashmap_new(size_t key_size, 
                                         size_t buck_len, size_t hash(void *)) {
  platform_hashmap_t *this =
      (platform_hashmap_t *)malloc(sizeof(platform_hashmap_t));
  if (!this)
    return NULL;
  this->key_size = key_size;
  this->buck_len = buck_len;
  if (hash)
    this->hash = hash;
  else
    this->hash = platform_hashmap_defalthash;

  this->buck =
      (platform_list_head_t **)calloc(buck_len, sizeof(platform_list_head_t *));
  return this;
}

void platform_hashmap_set(platform_hashmap_t *this, void *key, void *value, size_t value_size) {

  platform_assert(this);
  platform_assert(key);
  platform_assert(value);
  size_t index = this->hash(key);
  /*an item include a list_head,key and value*/
  platform_list_head_t *item =
      malloc(sizeof(platform_list_head_t) + this->key_size + value_size);
  if (!item)
    platform_abort();
  memcpy((char *)item + sizeof(platform_list_head_t), key, this->key_size);
  memcpy((char *)item + sizeof(platform_list_head_t) + this->key_size, value,
         value_size);
  if (this->buck[index]) {
    /*latest visited item always placed at start*/
    platform_list_head_insert_prev(*(this->buck + index), item);
  } else {
    item->prev = NULL;
    item->next = NULL;
  }
  this->buck[index] = item;
}

static platform_list_head_t *platform_hashmap_find(platform_hashmap_t *this,
                                                   void *key) {
  size_t index = this->hash(key);
  platform_list_head_t *head = *(this->buck + index);
  while (head) {
    if (platform_byteCmp((uint8_t *)(head + 1), key, this->key_size)) {
      return head;
    }
    head = platform_list_head_next(head);
  }
  return NULL;
}

void const *platform_hashmap_get(platform_hashmap_t *this, void *key) {
  platform_assert(this);
  platform_assert(key);
  platform_list_head_t *head = platform_hashmap_find(this, key);
  if (head) {
    /*move the item to start of the list*/
    platform_list_head_remove(head);
    platform_list_head_insert_prev(*(this->buck + this->hash(key)), head);
    /*return thie value*/
    return ((uint8_t *)(head + 1) + this->key_size);
  }
  return NULL;
}

void platform_hashmap_remove(platform_hashmap_t *this, void *key) {
  platform_assert(this);
  platform_assert(key);
  platform_list_head_t *head = platform_hashmap_find(this, key);
  if (head) {
    /*remove the head from list*/
    platform_list_head_remove(head);

    /*free the memory*/
    free(head);
  }
}

void platform_hashmap_delete(platform_hashmap_t *this){
	platform_assert(this);
	for(size_t i=0;i<this->buck_len;++i){
		platform_list_head_t *head = *(this->buck + i);
		platform_list_head_t *temp;
		int test = 0;
		while (head)
		{
			temp = platform_list_head_next(head);
			free(head);
			head = temp;
			++test;
		}
	}
	free(this->buck);
	free(this);
}
