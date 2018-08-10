#include "platform_bstree.h"

platform_bstree_t *platform_bstree_new(void *data,
                                       const platform_bstree_class_t *c) {
  platform_bstree_t *item =
      (platform_bstree_t *)malloc(sizeof(platform_bstree_t));
  if (item) {
    item->data = data;
    item->c = c;
    item->left = NULL;
    item->right = NULL;
    item->parent = NULL;
  }
  return item;
}

void platform_bstree_insert(platform_bstree_t *this, void *data) {
  platform_assert(this);
  platform_assert(data);
  platform_bstree_t *temp = this;
  platform_bstree_t *item =
      (platform_bstree_t *)malloc(sizeof(platform_bstree_t));
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
        temp = platform_bstree_item_left(temp);
      } else {
        temp->left = item;
        item->parent = temp;
        return;
      }
    } else if (res > 0) {
      /*item > temp ,choose right tree*/
      if (temp->right) {
        temp = platform_bstree_item_right(temp);
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
platform_bstree_t *platform_bstree_find(platform_bstree_t *this, void *data) {
  platform_bstree_t *temp = this;
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

platform_bstree_t *platform_bstree_minimum(platform_bstree_t *this) {
  if (this == NULL)
    return this;
  platform_bstree_t *temp = this;
  while (temp->left) {
    temp = temp->left;
  }
  return temp;
}

platform_bstree_t *platform_bstree_maximum(platform_bstree_t *this) {
  if (this == NULL)
    return this;
  platform_bstree_t *temp = this;
  while (temp->right) {
    temp = temp->right;
  }
  return temp;
}

platform_bstree_t *platform_bstree_predecessor(const platform_bstree_t *tree) {
  if (tree->left) {
    return platform_bstree_maximum(tree->left);
  }

  while (tree->parent && tree == tree->parent->left) {
    tree = tree->parent;
  }
  return tree->parent;
}

platform_bstree_t *platform_bstree_successor(platform_bstree_t *tree) {
  if (tree->right) {
    return platform_bstree_minimum(tree->right);
  }

  while (tree->parent && tree == tree->parent->right) {
    tree = tree->parent;
  }
  return tree->parent;
}

static platform_bstree_t *platform_bstree_remove_item(platform_bstree_t *tree,
                                                      platform_bstree_t *item) {
  platform_bstree_t *x;
  if (!item->left) {
    x = item->right;
  } else if (!item->right) {
    x = item->left;
  } else {
    x = platform_bstree_minimum(item->right);
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

platform_bstree_t *platform_bstree_remove(platform_bstree_t *tree, void *data) {
  platform_bstree_t *temp;
  while (temp = platform_bstree_find(tree, data)) {
    tree = platform_bstree_remove_item(tree, temp);
  }
  return tree;
}

#ifndef NODEBUG
void platform_bstree_print2(platform_bstree_t *tree, int blk) {
  for (int i = 0; i < blk; ++i)
    printf(" ");
  printf("|--");
  if (!tree) {
    printf("*");
    return;
  }
  tree->c->print_value(tree->data);
  printf("\n ");
  platform_bstree_print2(tree->left, blk + 1);
  printf("\n ");
  platform_bstree_print2(tree->right, blk + 1);
  // printf("\n");
}

void platform_bstree_print(platform_bstree_t *tree) {
  if (!tree)
    return;
  platform_bstree_print(tree->left);
  tree->c->print_value(tree->data);
  printf(" ");
  platform_bstree_print(tree->right);
}

#endif
