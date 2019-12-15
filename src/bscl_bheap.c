#include "bscl_bheap.h"
#include <stddef.h>

#define bscl_bheap_lc(i) ((i)*2 + 1)
#define bscl_bheap_rc(i) ((i)*2 + 2)
#define bscl_bheap_p(i) ((i - 1) / 2)

#define bscl_bheap_compare(h, x, y) ((h)->vptr->compare(x, y))
#define bscl_bheap_swap(h, x, y) ((h)->vptr->swap(x, y))
#define bscl_bheap_put(h, pos, data) ((h)->vptr->put(pos, data))
#define bscl_bheap_capacity(h) ((h)->vptr->capacity)
#define bscl_bheap_size(h) ((h)->size)

int bscl_bheap_construct(bscl_bheap_t *h, int start);

static void bscl_bheap_sift_down(bscl_bheap_t *h, int start) {
  int c = start;
  while (c < h->size) {
    int min;
    int l = bscl_bheap_lc(c);
    int r = bscl_bheap_rc(c);
    if (l >= h->size) {
      break;
    }

    if (r >= h->size) {
      min = l;
    } else {
      if (bscl_bheap_compare(h, l, r) < 0) {
        min = l;
      } else {
        min = r;
      }
    }

    if (bscl_bheap_compare(h, c, min) > 0) {
      bscl_bheap_swap(h, c, min);
    } else {
      break;
    }
  }
}

static void bscl_bheap_sift_up(bscl_bheap_t *h, int start) {
  int c = start;
  while (c > 0) {
    int p = bscl_bheap_p(c);
    if (bscl_bheap_compare(h, c, p) < 0) {
      bscl_bheap_swap(h, c, p);
      c = p;
    } else {
      break;
    }
  }
}

static void bscl_bheapity(bscl_bheap_t *h) {
  for (int i = bscl_bheap_p(h->size - 1); i >= 0; --i) {
    bscl_bheap_sift_down(h, i);
  }
}

int bscl_bheap_insert(bscl_bheap_t *h, void *data) {
  int pos = h->size;
  if (pos >= bscl_bheap_capacity(h)) {
    return -1;
  }
  bscl_bheap_put(h, pos, data);
  bscl_bheap_sift_up(h, pos);
  ++h->size;
  return 0;
}

int bscl_bheap_init(bscl_bheap_t *h, bscl_bheap_vtable_t *vptr, int size) {
  h->vptr = vptr;
  h->size = size;
  bscl_bheapity(h);
  return 0;
}

void bscl_bheap_deinit(bscl_bheap_t *h) {
  h->vptr = NULL;
  h->size = 0;
}

void bscl_bheap_remove(bscl_bheap_t *h, int n) {
  int last = h->size - 1;
  if (n > last) {
    return;
  }

  if (n < last) {
    bscl_bheap_swap(h, n, last);
    --h->size;
    bscl_bheap_sift_down(h, n);
  } else {
    h->size--;
  }
}
