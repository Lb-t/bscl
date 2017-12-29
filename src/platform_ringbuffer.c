#include "platform_ringbuffer.h"

size_t platform_ringbuffer_read(platform_ringbuffer_t *this, void *data,
                                size_t size) {
  platform_assert(this);
  platform_assert(size);
  size = size < platform_ringbuffer_size(this) ? size
                                               : platform_ringbuffer_size(this);
  const char *rbuf = platform_ringbuffer_get_readbuf(this);
  if ((rbuf + size) <= ((char *)this->data + this->capacity)) {
    memcpy(data, rbuf, size);
  } else {
    size_t s1 = (char *)this->data + this->capacity - rbuf;
    memcpy(data, rbuf, s1);
    memcpy((char *)data + s1, this->data, size - s1);
  }
  this->rIndex += size;
  return size;
}

size_t platform_ringbuffer_write(platform_ringbuffer_t *this, void *data,
                                 size_t size) {
  platform_assert(this);
  platform_assert(data);
  platform_assert(size);
  size = size < (this->capacity - platform_ringbuffer_size(this))
             ? size
             : (this->capacity - platform_ringbuffer_size(this));
  char *wbuf = platform_ringbuffer_get_writebuf(this);
  if ((wbuf + size) <= ((char *)this->data + this->capacity)) {
    memcpy(wbuf, data, size);
  } else {
    size_t s1 = (char *)this->data + this->capacity - wbuf;
    memcpy(wbuf, data, s1);
    memcpy((char *)this->data + s1, data, size - s1);
  }
  this->wIndex += size;
  return size;
}
