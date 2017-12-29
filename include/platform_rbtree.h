#ifndef INCLUDE_PLATFORM_RBTREE_H_
#define INCLUDE_PLATFORM_RBTREE_H_

typedef enum {
  PLATFORM_RBTREE_BLACK,
  PLATFORM_RBTREE_RED
} platform_rbtree_color_t;

typedef struct platform_rbtree_item_t_ {
  struct platform_rbtree_item_t_ *p, *lc, *rc; // parent,left child,right child
  platform_rbtree_color_t color;
} platform_rbtree_item_t;

#define platform_rbtree_insert_as_p()
#define platform_rbtree_insert_as_lc()
#define platform_rbtree_insert_as_rc()
#define platform_rbtree_remove()
#endif
