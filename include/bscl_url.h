#pragma once
#define QUERY_NUM_MAX 16
#ifndef PATH_MAX
#define PATH_MAX 128
#endif
typedef struct {
  char key[PATH_MAX];
  char value[PATH_MAX];
} bscl_url_query_t;

typedef struct {
  char path[PATH_MAX];
  unsigned int query_num;
  bscl_url_query_t *query[QUERY_NUM_MAX];
} bscl_url_t;

bscl_url_t *url_new_from_text(const char *text);
