#ifndef INCLUDE_PLATFORM_VECTOR_H_
#define INCLUDE_PLATFORM_VECTOR_H_
#include "../common/platform_common.h"

typedef struct {
  void *data;
  size_t type_size;
  size_t capacity;
  size_t size;
} platform_vector_t;

static inline size_t platform_vector_capacity(platform_vector_t *this) {
  return (this->capacity);
}
static inline size_t platform_vector_size(platform_vector_t *this) {
  return this->size;
}
static inline void *platform_vector_at(platform_vector_t *this, size_t index) {
  platform_assert(this);
  platform_assert(index < this->size);
  return (char *)this->data + index * this->type_size;
}
#define platform_vector_get(this, index)                                       \
  (void const *)((char *)this->data + type_size * index)
void platform_vector_remove(platform_vector_t *this, size_t index,
                            size_t count);
void platform_vector_insert(platform_vector_t *this, size_t index, size_t count,
                            void const *data);
void platform_vector_set(platform_vector_t *this, size_t start, size_t count,
                         void const *data);
void platform_vector_delete(platform_vector_t *this);
platform_vector_t *platform_vector_new(size_t initial_size, void *initial_value,
                                       size_t type_size);
#endif
