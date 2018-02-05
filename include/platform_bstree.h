#ifndef INCLUDE_PLATFORM_BSTREE_H_
#define INCLUDE_PLATFORM_BSTREE_H_
#include "platform_common.h"

typedef struct {
  int (*compare)(void *d1,
                 void *d2); // d1<d2 return -1 d1=d2 return 0 d1>d2 return 1
#ifndef NODEBUG
  void (*print_value)(void *d);
#endif
  size_t size;
} platform_bstree_class_t;

typedef struct platform_bstree_t_ {
  const platform_bstree_class_t *c;
  struct platform_bstree_t_ *left;   //
  struct platform_bstree_t_ *right;  //
  struct platform_bstree_t_ *parent; //
  void *data;
} platform_bstree_t;

platform_bstree_t *platform_bstree_new(void *data,
                                       const platform_bstree_class_t *c);
void platform_bstree_insert(platform_bstree_t *this, void *data);
platform_bstree_t *platform_bstree_find(platform_bstree_t *this, void *data);
void platform_bstree_print(platform_bstree_t *tree);
#define platform_bstree_item_data(item) (item->data)
#define platform_bstree_item_size(item) (item->size)
#define platform_bstree_item_parent(item) (item->parent)
#define platform_bstree_item_left(item) (item->left)
#define platform_bstree_item_right(item) (item->right)
#endif
