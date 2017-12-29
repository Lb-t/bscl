#ifndef INCLUDE_PLATFORM_BSTREE_H_
#define INCLUDE_PLATFORM_BSTREE_H_
#include "platform_common.h"
typedef struct platform_bstree_item_t_ {
  void *data;
  size_t size;
  struct platform_bstree_item_t_ *left;   //
  struct platform_bstree_item_t_ *right;  //
  struct platform_bstree_item_t_ *parent; //
} platform_bstree_item_t;

typedef struct {
  int (*compare)(void *d1, size_t s1, void *d2,
                 size_t s2); // d1<d2 return -1 d1=d2 return 0 d1>d2 return 1
  platform_bstree_item_t *root;
} platform_bstree_t;

platform_bstree_item_t *platform_bstree_item_new(void *data, size_t size);
platform_bstree_t *platform_bstree_new(int (*compare)(void *d1, size_t s1,
                                                      void *d2, size_t s2));
void platform_bstree_insert(platform_bstree_t *this,
                            platform_bstree_item_t *item);
platform_bstree_item_t *platform_bstree_find(platform_bstree_t *this,
                                             void *data, size_t size);

#define platform_bstree_item_data(item) (item->data)
#define platform_bstree_item_size(item) (item->size)
#define platform_bstree_item_parent(item) (item->parent)
#define platform_bstree_item_left(item) (item->left)
#define platform_bstree_item_right(item) (item->right)
#endif
