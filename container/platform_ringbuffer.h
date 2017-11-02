#ifndef INCLUDE_RINGBUFFER_H_
#define INCLUDE_RINGBUFFER_H_
#include "common/platform_common.h"
typedef struct {
  void *data;
  size_t capacity;
  volatile size_t rIndex;
  volatile size_t wIndex;
} platform_ringbuffer_t;

#define platform_ringbuffer_init(this, d, s)                                   \
  do {                                                                         \
    platform_assert(this);                                                     \
    platform_assert(!(s & (s - 1)));                                           \
    this->capacity = s;                                                        \
    this->data = d;                                                            \
    this->rIndex = 0;                                                          \
    this->wIndex = 0;                                                          \
  } while (0)
#define platform_ringbuffer_get_readbuf(this)                                  \
  (((const char *)this->data + (this->rIndex & (this->capacity - 1))))

#define platform_ringbuffer_get_writebuf(this)                                 \
  (((char *)this->data + (this->wIndex & (this->capacity - 1))))

#define platform_ringbuffer_size(this) ((this)->wIndex - (this)->rIndex)

#define platform_ringbuffer_capacity(this) ((this)->capacity)

size_t platform_ringbuffer_read(platform_ringbuffer_t *this, void *data,
                                size_t size);
size_t platform_ringbuffer_write(platform_ringbuffer_t *this, void *data,
                                 size_t size);
#define platform_ringbuffer_clear(this)                                        \
  do {                                                                         \
    (this)->rIndex = (this)->wIndex;                                           \
  } while (0)

#endif
