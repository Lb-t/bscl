#include "platform_bstree.h"
#include <stdio.h>
int compare(void *d1, void *d2) {
  int v1 = *(int *)d1;
  int v2 = *(int *)d2;
  if (v1 == v2)
    return 0;
  else if (v1 < v2)
    return -1;
  else
    return 1;
}

void print_value(void *d) { printf("%d ", *(int *)d); }

const platform_bstree_class_t bstree_int = {
    .compare = compare, .print_value = print_value, .size = sizeof(int)};

int main(void) {

  int *value = malloc(sizeof(int));

  *value = 0;

  platform_bstree_t *tree = platform_bstree_new(value, &bstree_int);
  // return 0;

  for (int i = 1; i < 50; ++i) {

    value = malloc(sizeof(int));

    *value = rand() % 50;
    printf("for %d %d \n", *value, value);
    fflush(stdout);
    platform_bstree_insert(tree, value);
  }

  platform_bstree_print(tree);

  printf("\n");
  for (int i = 0; i < 50; ++i) {
    int value = rand() % 50;
    printf("\nvalue to remove %d \n", value);
    tree = platform_bstree_remove(tree, &value);
    platform_bstree_print(tree);
    printf("\n");
  }

  return 0;
}