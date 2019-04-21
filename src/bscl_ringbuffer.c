#include "bscl_ringbuffer.h"

size_t bscl_ringbuffer_read(bscl_ringbuffer_t *this, void *data, size_t len) {
  bscl_assert(this);
  bscl_assert(len);
  size_t size = bscl_ringbuffer_size(this);
  len = len < size ? len : size;
  const char *rbuf = bscl_ringbuffer_get_readbuf(this);
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

size_t bscl_ringbuffer_write(bscl_ringbuffer_t *this, void *data, size_t len) {
  bscl_assert(this);
  bscl_assert(data);
  bscl_assert(len);
  size_t space = this->capacity - bscl_ringbuffer_size(this);
  len = len < space ? len : space;
  char *wbuf = bscl_ringbuffer_get_writebuf(this);
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
