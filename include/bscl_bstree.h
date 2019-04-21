#pragma once
#include "bscl_common.h"

typedef struct {
  int (*compare)(void *d1,
                 void *d2); // d1<d2 return -1 d1=d2 return 0 d1>d2 return 1
#ifndef NODEBUG
  void (*print_value)(void *d);
#endif
  size_t size;
} bscl_bstree_class_t;

typedef struct bscl_bstree_t_ {
  const bscl_bstree_class_t *c;
  struct bscl_bstree_t_ *left;   //
  struct bscl_bstree_t_ *right;  //
  struct bscl_bstree_t_ *parent; //
  void *data;
} bscl_bstree_t;

bscl_bstree_t *bscl_bstree_new(void *data,
                                       const bscl_bstree_class_t *c);
void bscl_bstree_insert(bscl_bstree_t *this, void *data);
bscl_bstree_t *bscl_bstree_find(bscl_bstree_t *this, void *data);
void bscl_bstree_print(bscl_bstree_t *tree);
bscl_bstree_t *bscl_bstree_remove(bscl_bstree_t *tree, void *data);
#define bscl_bstree_item_data(item) (item->data)
#define bscl_bstree_item_size(item) (item->size)
#define bscl_bstree_item_parent(item) (item->parent)
#define bscl_bstree_item_left(item) (item->left)
#define bscl_bstree_item_right(item) (item->right)
