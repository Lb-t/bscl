#include "platform_vector.h"
#include "platform_common.h"
#include <limits.h>
#include <stddef.h>

/*create a new vector with given initial size and value*/
platform_vector_t *platform_vector_new(size_t initial_size, void *initial_value, size_t type_size) {
  platform_assert(type_size);
  platform_assert(!initial_size || initial_value);
  platform_vector_t *vector = (platform_vector_t *)malloc(sizeof(platform_vector_t));
  if (!vector) {
    return NULL;
  }
  if (!initial_size) {
    vector->capacity = 1;
  } else {
    vector->capacity = 1ull << (sizeof(size_t) * CHAR_BIT - 1);
    while (vector->capacity != 1 && (!(vector->capacity & initial_size))) {
      vector->capacity = vector->capacity >> 1;
    }
    vector->capacity = vector->capacity << 1;
  }
  vector->type_size = type_size;
  vector->size = initial_size;
  vector->data = malloc(vector->capacity * type_size);
  if (!vector->data) {
    free(vector);
    return NULL;
  }
  for (size_t i = 0; i < initial_size; ++i) {
    memcpy((char *)vector->data + i * type_size, initial_value, type_size);
  }
  return vector;
}
#if 0
size_t platform_vector_capacity(platform_vector_t const *this) {
  platform_assert(this);
  return this->capacity;
}

size_t platform_vector_size(platform_vector_t const *this) {
  platform_assert(this);
  return this->size;
}

/*read out data that started at start,count item*/
void platform_vector_get(platform_vector_t const *this, size_t start,
                         size_t count, void *data) {
  platform_assert(this);
  platform_assert(count);
  platform_assert(start + count <= this->size);
  memcpy(data, (char *)this->data + start * this->type_size,
         this->type_size * count);
}
#endif
void platform_vector_set(platform_vector_t *this, size_t start, size_t count, void const *data) {
  platform_assert(this);
  platform_assert(count);
  platform_assert(start + count <= this->size);
  memcpy((char *)this->data + start * this->type_size, data, this->type_size * count);
}

void platform_vector_insert(platform_vector_t *this, size_t index, size_t count, void const *data) {
  platform_assert(this);
  platform_assert(data);
  platform_assert(count);
  platform_assert(index <= this->size);
  if (this->size + count > this->capacity) {
    // printf("before realloc capacity=%d msize=%d\n", this->capacity,
    //         _msize(this->data));
    while (this->size + count > this->capacity)
      this->capacity = this->capacity << 1;

    void *temp = realloc(this->data, this->capacity * this->type_size);
    if (!temp) {
      perror("realloc error:");
      exit(0);
      temp = malloc(this->capacity * this->type_size);
      if (!temp) {
        perror("malloc error:");
        exit(0);
      }
      free(this->data);
      this->data = temp;
    } else {
      this->data = temp;
    }
  }

  // printf(" insert.capacity=%d size=%d msize=%d\n", this->capacity,
  // this->size, _msize(this->data));
  // printf("insert capacity=%d size=%d data=%x\n", this->capacity, this->size,
  // this->data);

  // platform_assert(this->capacity * this->type_size <= _msize(this->data));
  platform_assert(index + count <= this->capacity);
  memmove((char *)this->data + (index + count) * this->type_size, (char *)this->data + index * this->type_size,
          (this->size - index) * this->type_size);
  memcpy((char *)this->data + index * this->type_size, data, count * this->type_size);

  this->size += count;
}

void platform_vector_remove(platform_vector_t *this, size_t index, size_t count) {
  platform_assert(this);
  platform_assert(count);
  platform_assert(index + count <= this->size);
  // printf("remove capacity=%d size=%d \n", this->capacity, this->size);
  memmove((char *)this->data + index * this->type_size, (char *)this->data + (index + count) * this->type_size,
          (this->size - index - count) * this->type_size);
  this->size -= count;
  if (this->size < (this->capacity >> 1)) {
    this->capacity = this->capacity >> 1;
    this->data = realloc(this->data, this->capacity * this->type_size);
    if (!this->data) {
      platform_abort();
    }
  }
}

void platform_vector_delete(platform_vector_t *this) {
  platform_assert(this);
  free(this->data);
  free(this);
}
