#include "platform_bstree.h"

platform_bstree_item_t *platform_bstree_item_new(void *data, size_t size) {
  platform_bstree_item_t *item =
      (platform_bstree_item_t *)malloc(sizeof(platform_bstree_item_t));
  if (item) {
    item->size = size;
    item->data = data;
  }
  return item;
}
platform_bstree_t *platform_bstree_new(int (*compare)(void *d1, size_t s1,
                                                      void *d2, size_t s2)) {
  platform_bstree_t *tree =
      (platform_bstree_t *)malloc(sizeof(platform_bstree_t));
  if (tree) {
    tree->compare = compare;
    tree->root = NULL;
  }
}
void platform_bstree_insert(platform_bstree_t *this,
                            platform_bstree_item_t *item) {
  platform_assert(this);
  platform_assert(item);
  platform_bstree_item_t *temp = this->root;
  int res;
  if (!temp) {
    this->root = item;
    return;
  }
  while (temp) {
    res = this->compare(item->data, item->size, temp->data, temp->size);
    if (res < 0) {
      /*item <temp,choose left tree*/
      if (temp->left) {
        temp = platform_bstree_item_left(temp);
      } else {
        temp->left = item;
        item->parent = temp;
        return;
      }
    } else if (temp > 0) {
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
      item->left->parent = item;
      temp->left = item;
      item->parent = temp;
      return;
    }
  }
}
platform_bstree_item_t *platform_bstree_find(platform_bstree_t *this,
                                             void *data, size_t size) {
  platform_bstree_item_t *temp = this->root;
  int res;
  while (temp) {
    res = this->compare(data, size, temp->data, temp->size);
    if (res < 0) {
      temp = temp->left;
    } else if (temp > 0) {
      temp = temp->right;
    } else {
      break;
    }
  }
  return temp;
}
