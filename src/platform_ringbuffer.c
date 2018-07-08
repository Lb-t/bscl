#include "platform_ringbuffer.h"

size_t platform_ringbuffer_read(platform_ringbuffer_t *this, void *data,
                                size_t len) {
  platform_assert(this);
  platform_assert(len);
  size_t size = platform_ringbuffer_size(this);
  len = len < size ? len : size;
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
                                 size_t len) {
  platform_assert(this);
  platform_assert(data);
  platform_assert(len);
  size_t space = this->capacity - platform_ringbuffer_size(this);
  len = len < space ? len : space;
  char *wbuf = platform_ringbuffer_get_writebuf(this);
  if ((wbuf + len) <= ((char *)this->data + this->capacity)) {
    memcpy(wbuf, data, len);
  } else {
    size_t s1 = (char *)this->data + this->capacity - wbuf;
    memcpy(wbuf, data, s1);
    memcpy(this->data, (char *)data + s1, len - s1);
  }
  this->wIndex += len;
  return len;
}
