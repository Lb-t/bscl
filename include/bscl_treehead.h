#ifndef INCLUDE_PLATFORM_TREE_H_
#define INCLUDE_PLATFORM_TREE_H_
typedef struct platform_tree_head_t_ {
  struct platform_tree_head_t_ *parent, *left, *right;
} platform_tree_head_t;

#define platform_tree_head_data(tree, mem, p) container_of(p, s, mem)
#define platform_tree_head_parent(tree) (tree->parent)
#define platform_tree_head_left(tree) (tree->left)
#define platform_tree_head_right(tree) (tree->right)
#define platform_tree_insert(tree, newHead)                                    \
  do {                                                                         \
  } while (0)
#define platform_tree_remove(head)
#endif