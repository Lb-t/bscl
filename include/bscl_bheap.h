#pragma once

typedef struct {
  void (*swap)(int x, int y);
  void (*put)(int pos,void*data);
  int (*compare)(int x, int y);
  int capacity;
} bscl_bheap_vtable_t;

typedef struct {
  const bscl_bheap_vtable_t *vptr;
  int size;
} bscl_bheap_t;

void bscl_bheap_deinit(bscl_bheap_t *h);
int bscl_bheap_init(bscl_bheap_t *h, bscl_bheap_vtable_t *vptr, int size);
int bscl_bscl_bheap_insert(bscl_bheap_t *h, void *data);
void bscl_bscl_bheap_remove(bscl_bheap_t *h, int n);


