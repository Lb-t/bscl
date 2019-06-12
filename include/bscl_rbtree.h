#pragma once

typedef enum { BSCL_RBTREE_RED, BSCL_RBTREE_BLACK } bscl_rbtree_color_t;

typedef struct bscl_rbtree_t_ {
  struct bscl_rbtree_t_ *p;
  struct bscl_rbtree_t_ *l;
  struct bscl_rbtree_t_ *r;
} bscl_rbtree_t;
