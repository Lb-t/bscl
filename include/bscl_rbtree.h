#pragma once

typedef enum { BSCL_RBTREE_RED, BSCL_RBTREE_BLACK } bscl_rbtree_color_t;

typedef struct bscl_rbtree_node_t_ {
  struct bscl_rbtree_node_t_ *p;
  struct bscl_rbtree_node_t_ *l;
  struct bscl_rbtree_node_t_ *r;
  bscl_rbtree_color_t color;
} bscl_rbtree_node_t;

typedef int (*bscl_rbtree_comp_t)(bscl_rbtree_node_t *x, bscl_rbtree_node_t *y);

typedef struct {
  bscl_rbtree_node_t *root;
  bscl_rbtree_comp_t comp;
} bscl_rbtree_t;
