/*
 * list.c
 */
#include "platform_list.h"
#define _TIMESPEC_DEFINED
#include <pthread.h>
struct platform_list_item_t_ {
	struct platform_list_item_t_ *next;
	struct platform_list_item_t_ *prev;
	void *data;
	size_t len;
};

struct platform_list_t_ {
	platform_list_item_t *head;
	platform_list_item_t *tail;
	pthread_mutex_t mutex;
	size_t size;
};

/*insert a new item after item*/
bool platform_list_insert(platform_list_t *list, platform_list_item_t *item, void *data,
		size_t len) {
	platform_assert(list);
	 platform_list_item_t *newItem = (platform_list_item_t *) malloc(
			sizeof(platform_list_item_t));
	if (!newItem)
		return false;

	pthread_mutex_lock(&list->mutex);
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
	pthread_mutex_unlock(&list->mutex);

	newItem->data = data;
	newItem->len = len;
	return true;
}

bool platform_list_remove(platform_list_t *list, platform_list_item_t *item) {
	platform_assert(list);
	platform_assert(item);
	if ((!list->size))
		return false;

	pthread_mutex_lock(&list->mutex);
	if (NULL == item->prev)
		list->head = item->next;
	else
		item->prev->next = item->next;
	if (NULL == item->next)
		list->tail = item->prev;
	else
		item->next->prev = item->prev;
	list->size -= 1;
	pthread_mutex_unlock(&list->mutex);

	free(item);
	return true;
}

void platform_list_init(platform_list_t *list) {
	platform_assert(list);
	pthread_mutex_init(&list->mutex,NULL);
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
}

void platform_list_deninit(platform_list_t *list) {
	platform_assert(list);
	platform_list_item_t *item = list->head;
	pthread_mutex_lock(&list->mutex);
	while (item) {
		item = item->next;
		free(item->prev);
	}
	pthread_mutex_unlock(&list->mutex);
	pthread_mutex_destroy(&list->mutex);
}

platform_list_t *platform_list_new(void) {
	platform_list_t *newList = (platform_list_t *) malloc(sizeof(platform_list_t));
	if (newList)
		platform_list_init(newList);
	return newList;
}

void platform_list_delete(platform_list_t *list) {
	platform_assert(list);
	platform_list_deninit(list);
	free(list);
}

size_t platform_list_item_data(platform_list_item_t *item, void **data) {
	platform_assert(item);
	*data = item->data;
	return item->len;
}
platform_list_item_t *platform_list_item_prev(platform_list_item_t *item) {
	platform_assert(item);
	return item->prev;
}
platform_list_item_t *platform_list_item_next(platform_list_item_t *item) {
	platform_assert(item);
	return item->next;
}
platform_list_item_t *platform_list_head(platform_list_t *list) {
	platform_assert(list);
	return list->head;
}
platform_list_item_t *platform_list_tail(platform_list_t *list) {
	platform_assert(list);
	return list->tail;
}
size_t platform_list_size(platform_list_t *list) {
	platform_assert(list);
	return list->size;
}
