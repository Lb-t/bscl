#include "bscl_hashmap.h"
#include "bscl_common.h"
#include <stdint.h>
#include <stdlib.h>
size_t bscl_hashmap_defalthash(void *key) { return 0; }

bscl_hashmap_t *bscl_hashmap_new(size_t key_size, size_t buck_len, size_t hash(void *)) {
  bscl_hashmap_t *this = (bscl_hashmap_t *)malloc(sizeof(bscl_hashmap_t));
  if (!this)
    return NULL;
  this->key_size = key_size;
  this->buck_len = buck_len;
  if (hash)
    this->hash = hash;
  else
    this->hash = bscl_hashmap_defalthash;

  this->buck = (bscl_list_head_t **)calloc(buck_len, sizeof(bscl_list_head_t *));
  return this;
}

void bscl_hashmap_set(bscl_hashmap_t *this, void *key, void *value, size_t value_size) {

  bscl_assert(this);
  bscl_assert(key);
  bscl_assert(value);
  size_t index = this->hash(key);
  /*an item include a list_head,key and value*/
  bscl_list_head_t *item = malloc(sizeof(bscl_list_head_t) + this->key_size + value_size);
  if (!item)
    bscl_abort();
  memcpy((char *)item + sizeof(bscl_list_head_t), key, this->key_size);
  memcpy((char *)item + sizeof(bscl_list_head_t) + this->key_size, value, value_size);
  if (this->buck[index]) {
    /*latest visited item always placed at start*/
    bscl_list_head_insert_prev(*(this->buck + index), item);
  } else {
    item->prev = NULL;
    item->next = NULL;
  }
  this->buck[index] = item;
}

static bscl_list_head_t *bscl_hashmap_find(bscl_hashmap_t *this, void *key) {
  size_t index = this->hash(key);
  bscl_list_head_t *head = *(this->buck + index);
  while (head) {
    if (bscl_byteCmp((uint8_t *)(head + 1), key, this->key_size)) {
      return head;
    }
    head = bscl_list_head_next(head);
  }
  return NULL;
}

void const *bscl_hashmap_get(bscl_hashmap_t *this, void *key) {
  bscl_assert(this);
  bscl_assert(key);
  bscl_list_head_t *head = bscl_hashmap_find(this, key);
  if (head) {
    /*move the item to start of the list*/
    bscl_list_head_remove(head);
    bscl_list_head_insert_prev(*(this->buck + this->hash(key)), head);
    /*return thie value*/
    return ((uint8_t *)(head + 1) + this->key_size);
  }
  return NULL;
}

void bscl_hashmap_remove(bscl_hashmap_t *this, void *key) {
  bscl_assert(this);
  bscl_assert(key);
  bscl_list_head_t *head = bscl_hashmap_find(this, key);
  if (head) {
    /*remove the head from list*/
    bscl_list_head_remove(head);

    /*free the memory*/
    free(head);
  }
}

void bscl_hashmap_delete(bscl_hashmap_t *this) {
  bscl_assert(this);
  for (size_t i = 0; i < this->buck_len; ++i) {
    bscl_list_head_t *head = *(this->buck + i);
    bscl_list_head_t *temp;
    int test = 0;
    while (head) {
      temp = bscl_list_head_next(head);
      free(head);
      head = temp;
      ++test;
    }
  }
  free(this->buck);
  free(this);
}
