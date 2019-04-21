#pragma once
#include "bscl_common.h"
typedef struct {
  void *data;
  size_t capacity;
  volatile size_t rIndex;
  volatile size_t wIndex;
} bscl_ringbuffer_t;

#define bscl_ringbuffer_init(this, d, s)                                   \
  do {                                                                         \
    bscl_assert(this);                                                     \
    bscl_assert(!(s & (s - 1)));                                           \
    this->capacity = s;                                                        \
    this->data = d;                                                            \
    this->rIndex = 0;                                                          \
    this->wIndex = 0;                                                          \
  } while (0)
#define bscl_ringbuffer_get_readbuf(this)                                  \
  (((const char *)this->data + (this->rIndex & (this->capacity - 1))))

#define bscl_ringbuffer_get_writebuf(this)                                 \
  (((char *)this->data + (this->wIndex & (this->capacity - 1))))

#define bscl_ringbuffer_size(this) ((this)->wIndex - (this)->rIndex)

#define bscl_ringbuffer_capacity(this) ((this)->capacity)

size_t bscl_ringbuffer_read(bscl_ringbuffer_t *this, void *data,
                                size_t size);
size_t bscl_ringbuffer_write(bscl_ringbuffer_t *this, void *data,
                                 size_t size);
#define bscl_ringbuffer_clear(this)                                        \
  do {                                                                         \
    (this)->rIndex = (this)->wIndex;                                           \
  } while (0)

