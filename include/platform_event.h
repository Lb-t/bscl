#ifndef INCLUDE_PLATFORM_EVENT_H_
#define INCLUDE_PLATFORM_EVENT_H_

#include "platform_event.h"
#include "platform_queue.h"

#include "platform_os.h"
typedef struct {
  platform_os_sem_t sem;
  platform_queue_t *queue;
} platform_event_t;

#define platform_event_init(event)                                             \
  do {                                                                         \
    (event)->queue = platform_queue_new();                                     \
    platform_os_sem_create(&(event)->sem, 0);                                             \
  } while (0)

#define platform_event_deinit(event)                                           \
  do {                                                                         \
    platform_queue_delete((event)->queue);                                     \
    platform_os_sem_delete(&(event)->sem);                                                \
  } while (0)

#define platform_event_put(event, id)                                          \
  do {                                                                         \
    int *data = malloc(sizeof(int));                                           \
    *data = id;                                                                \
    platform_queue_push((event)->queue, data, sizeof(*data));                  \
    platform_os_sem_post(&(event)->sem);                                                   \
  } while (0)

static inline int platform_event_get(platform_event_t *const event) {
  platform_os_sem_wait(&(event)->sem);
  int id;
  size_t len;
    platform_queue_pop((event))->queue, &id, &len);
    return id;
}

#endif
