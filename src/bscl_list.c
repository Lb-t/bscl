/*
 * list.c
 */
#include "bscl_list.h"
#include "bscl_os.h"
struct bscl_list_item_t_ {
  struct bscl_list_item_t_ *next;
  struct bscl_list_item_t_ *prev;
  void *data;
  size_t len;
};

struct bscl_list_t_ {
  bscl_list_item_t *head;
  bscl_list_item_t *tail;
  bscl_os_mutex_t mutex;
  size_t size;
};

/*insert a new item after item*/
bool bscl_list_insert(bscl_list_t *list, bscl_list_item_t *item, void *data, size_t len) {
  bscl_assert(list);
  bscl_list_item_t *newItem = (bscl_list_item_t *)malloc(sizeof(bscl_list_item_t));
  if (!newItem)
    return false;

  bscl_os_mutex_lock(&list->mutex);
  if (NULL == item) {
    newItem->next = list->head;
    list->head = newItem;
    newItem->prev = NULL;
  } else {
    newItem->next = item->next;
    newItem->prev = item;
    item->next = newItem;
  }
  if (newItem->next)
    newItem->next->prev = newItem;
  else {
    list->tail = newItem;
  }
  list->size += 1;
  bscl_os_mutex_unlock(&list->mutex);

  newItem->data = data;
  newItem->len = len;
  return true;
}

bool bscl_list_remove(bscl_list_t *list, bscl_list_item_t *item) {
  bscl_assert(list);
  bscl_assert(item);
  if ((!list->size))
    return false;

  bscl_os_mutex_lock(&list->mutex);
  if (NULL == item->prev)
    list->head = item->next;
  else
    item->prev->next = item->next;
  if (NULL == item->next)
    list->tail = item->prev;
  else
    item->next->prev = item->prev;
  list->size -= 1;
  bscl_os_mutex_unlock(&list->mutex);

  free(item);
  return true;
}

void bscl_list_init(bscl_list_t *list) {
  bscl_assert(list);
  bscl_os_mutex_create(&list->mutex);
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
}

void bscl_list_deninit(bscl_list_t *list) {
  bscl_assert(list);
  bscl_list_item_t *item = list->head;
  bscl_os_mutex_lock(&list->mutex);
  while (item) {
    item = item->next;
    free(item->prev);
  }
  bscl_os_mutex_unlock(&list->mutex);
  bscl_os_mutex_delete(&list->mutex);
}

bscl_list_t *bscl_list_new(void) {
  bscl_list_t *newList = (bscl_list_t *)malloc(sizeof(bscl_list_t));
  if (newList)
    bscl_list_init(newList);
  return newList;
}

void bscl_list_delete(bscl_list_t *list) {
  bscl_assert(list);
  bscl_list_deninit(list);
  free(list);
}

size_t bscl_list_item_data(bscl_list_item_t *item, void **data) {
  bscl_assert(item);
  *data = item->data;
  return item->len;
}
bscl_list_item_t *bscl_list_item_prev(bscl_list_item_t *item) {
  bscl_assert(item);
  return item->prev;
}
bscl_list_item_t *bscl_list_item_next(bscl_list_item_t *item) {
  bscl_assert(item);
  return item->next;
}
bscl_list_item_t *bscl_list_head(bscl_list_t *list) {
  bscl_assert(list);
  return list->head;
}
bscl_list_item_t *bscl_list_tail(bscl_list_t *list) {
  bscl_assert(list);
  return list->tail;
}
size_t bscl_list_size(bscl_list_t *list) {
  bscl_assert(list);
  return list->size;
}
