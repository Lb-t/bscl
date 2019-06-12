#include "bscl_bstree.h"

bscl_bstree_t *bscl_bstree_new(void *data, const bscl_bstree_class_t *c) {
  bscl_bstree_t *item = (bscl_bstree_t *)malloc(sizeof(bscl_bstree_t));
  if (item) {
    item->data = data;
    item->c = c;
    item->left = NULL;
    item->right = NULL;
    item->parent = NULL;
  }
  return item;
}

void bscl_bstree_insert(bscl_bstree_t *this, void *data) {
  bscl_assert(this);
  bscl_assert(data);
  bscl_bstree_t *temp = this;
  bscl_bstree_t *item = (bscl_bstree_t *)malloc(sizeof(bscl_bstree_t));
  assert(item);
  item->data = data;
  item->left = NULL;
  item->right = NULL;
  item->c = this->c;
  int res;
  while (temp) {
    res = this->c->compare(data, temp->data);
    if (res < 0) {
      /*item <temp,choose left tree*/
      if (temp->left) {
        temp = bscl_bstree_item_left(temp);
      } else {
        temp->left = item;
        item->parent = temp;
        return;
      }
    } else if (res > 0) {
      /*item > temp ,choose right tree*/
      if (temp->right) {
        temp = bscl_bstree_item_right(temp);
      } else {
        temp->right = item;
        item->parent = temp;
        return;
      }
    } else {
      item->left = temp->left;
      item->parent = temp;
      if (item->left)
        item->left->parent = item;
      temp->left = item;
      return;
    }
  }
}
bscl_bstree_t *bscl_bstree_find(bscl_bstree_t *this, void *data) {
  bscl_bstree_t *temp = this;
  int res;
  while (temp) {
    res = this->c->compare(data, temp->data);
    if (res < 0) {
      temp = temp->left;
    } else if (res > 0) {
      temp = temp->right;
    } else {
      break;
    }
  }
  return temp;
}

bscl_bstree_t *bscl_bstree_minimum(bscl_bstree_t *this) {
  if (this == NULL)
    return this;
  bscl_bstree_t *temp = this;
  while (temp->left) {
    temp = temp->left;
  }
  return temp;
}

bscl_bstree_t *bscl_bstree_maximum(bscl_bstree_t *this) {
  if (this == NULL)
    return this;
  bscl_bstree_t *temp = this;
  while (temp->right) {
    temp = temp->right;
  }
  return temp;
}

bscl_bstree_t *bscl_bstree_predecessor(const bscl_bstree_t *tree) {
  if (tree->left) {
    return bscl_bstree_maximum(tree->left);
  }

  while (tree->parent && tree == tree->parent->left) {
    tree = tree->parent;
  }
  return tree->parent;
}

bscl_bstree_t *bscl_bstree_successor(bscl_bstree_t *tree) {
  if (tree->right) {
    return bscl_bstree_minimum(tree->right);
  }

  while (tree->parent && tree == tree->parent->right) {
    tree = tree->parent;
  }
  return tree->parent;
}

static bscl_bstree_t *bscl_bstree_remove_item(bscl_bstree_t *tree, bscl_bstree_t *item) {
  bscl_bstree_t *x;
  if (!item->left) {
    x = item->right;
  } else if (!item->right) {
    x = item->left;
  } else {
    x = bscl_bstree_minimum(item->right);
    if (x == item->right) {

    } else {
      if (x->right) {
        x->right->parent = x->parent;
      }
      x->parent->left = x->right;
      x->right = item->right;
      x->right->parent = x;
    }
    x->left = item->left;
    if (x->left)
      x->left->parent = x;
  }

  if (item == tree) {
    tree = x;
  } else {
    if (item->parent->left == item) {
      item->parent->left = x;
    } else {
      item->parent->right = x;
    }
  }
  if (x)
    x->parent = item->parent;
  free(item);
  return tree;
}

bscl_bstree_t *bscl_bstree_remove(bscl_bstree_t *tree, void *data) {
  bscl_bstree_t *temp;
  while ((temp = bscl_bstree_find(tree, data))) {
    tree = bscl_bstree_remove_item(tree, temp);
  }
  return tree;
}

#ifndef NODEBUG
void bscl_bstree_print2(bscl_bstree_t *tree, int blk) {
  for (int i = 0; i < blk; ++i)
    printf(" ");
  printf("|--");
  if (!tree) {
    printf("*");
    return;
  }
  tree->c->print_value(tree->data);
  printf("\n ");
  bscl_bstree_print2(tree->left, blk + 1);
  printf("\n ");
  bscl_bstree_print2(tree->right, blk + 1);
  // printf("\n");
}

void bscl_bstree_print(bscl_bstree_t *tree) {
  if (!tree)
    return;
  bscl_bstree_print(tree->left);
  tree->c->print_value(tree->data);
  printf(" ");
  bscl_bstree_print(tree->right);
}

#endif
