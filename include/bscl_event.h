#pragma once
#include "bscl_event.h"
#include "bscl_queue.h"

#include "bscl_os.h"
typedef struct {
  bscl_os_sem_t sem;
  bscl_queue_t *queue;
} bscl_event_t;

#define bscl_event_init(event)                                                                                                 \
  do {                                                                                                                         \
    (event)->queue = bscl_queue_new();                                                                                         \
    bscl_os_sem_create(&(event)->sem, 0);                                                                                      \
  } while (0)

#define bscl_event_deinit(event)                                                                                               \
  do {                                                                                                                         \
    bscl_queue_delete((event)->queue);                                                                                         \
    bscl_os_sem_delete(&(event)->sem);                                                                                         \
  } while (0)

#define bscl_event_put(event, id)                                                                                              \
  do {                                                                                                                         \
    int *data = malloc(sizeof(int));                                                                                           \
    *data = id;                                                                                                                \
    bscl_queue_push((event)->queue, data, sizeof(*data));                                                                      \
    bscl_os_sem_post(&(event)->sem);                                                                                           \
  } while (0)

static inline int bscl_event_get(bscl_event_t *const event) {
  bscl_os_sem_wait(&(event)->sem);
  int id;
  size_t len;
    bscl_queue_pop((event))->queue, &id, &len);
    return id;
}
