#pragma once
#include "bscl_common.h"

typedef struct {
  void *data;
  size_t type_size;
  size_t capacity;
  size_t size;
} bscl_vector_t;

static inline size_t bscl_vector_capacity(bscl_vector_t *this) {
  return (this->capacity);
}
static inline size_t bscl_vector_size(bscl_vector_t *this) {
  return this->size;
}
static inline void *bscl_vector_at(bscl_vector_t *this, size_t index) {
  bscl_assert(this);
  bscl_assert(index < this->size);
  return (char *)this->data + index * this->type_size;
}
#define bscl_vector_get(this, index)                                       \
  (void const *)((char *)this->data + type_size * index)
void bscl_vector_remove(bscl_vector_t *this, size_t index,
                            size_t count);
void bscl_vector_insert(bscl_vector_t *this, size_t index, size_t count,
                            void const *data);
void bscl_vector_set(bscl_vector_t *this, size_t start, size_t count,
                         void const *data);
void bscl_vector_delete(bscl_vector_t *this);
bscl_vector_t *bscl_vector_new(size_t initial_size, void *initial_value,
                                       size_t type_size);
