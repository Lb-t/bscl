#include "bscl_rbtree.h"
#include <stddef.h>
#include <assert.h>

#define is_left_child(node) ((node)->p->l == (node))
#define is_right_child(node) ((node)->p->r == (node))

static int left_rotate(bscl_rbtree_t *tree, bscl_rbtree_node_t *x) {
  bscl_rbtree_node_t *temp = x->r;

  x->r = temp->l;

  if (x->r != NULL) {
    x->r->p = x;
  }

  temp->l = x;
  temp->p = x->p;
  x->p = temp;

  if (temp->p == NULL) {
    tree->root = temp;
  } else {
    if (is_left_child(temp)) {
      temp->p->l = temp;
    } else if (is_right_child(node)) {
      temp->p->r = temp;
    } else {
      assert(0);
    }
  }

  return 0;
}

static int right_rotate(bscl_rbtree_t *tree, bscl_rbtree_node_t *x) {
  bscl_rbtree_node_t *temp = x->l;

  x->l = temp->r;

  if (x->l != NULL) {
    x->l->p = x;
  }

  temp->r = x;
  temp->p = x->p;
  x->p = temp;

  if (temp->p == NULL) {
    tree->root = temp;
  } else {
    if (is_left_child(temp)) {
      temp->p->l = temp;
    } else if (is_right_child(node)) {
      temp->p->r = temp;
    } else {
      assert(0);
    }
  }

  return 0;
}

int bscl_rbtree_init(bscl_rbtree_t *tree) {
  tree->root = NULL;
  return 0;
}

static int insert_fixup(bscl_rbtree_t *tree, bscl_rbtree_node_t *x)
{
  
}

int bscl_rbtree_insert(bscl_rbtree_t *tree, bscl_rbtree_node_t *x) {
  bscl_rbtree_node_t *where = tree->root;
  bscl_rbtree_node_t *p = where->p;
  while (where) {
    p = where;
    if (tree->comp(x, where) <= 0) {
      where = where->l;
    } else {
      where = where->r;
    }
  }

  x->l = NULL;
  x->r = NULL;
  if (p == NULL) {
    tree->root = x;
    x->p = NULL;
  } else {
    x->p = p;
    if (tree->comp(x, p) <= 0) {
      p->l = x;
    } else {
      p->r = x;
    }
  }

  x->color=BSCL_RBTREE_RED;


  return 0;
}

int bscl_rbtree_remove(bscl_rbtree_t *tree, bscl_rbtree_t *x) {
  return -1;
}
