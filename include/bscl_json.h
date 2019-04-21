#pragma once
#include "bscl_list_head.h"
#include <stdbool.h>
#include <stdlib.h>
typedef enum { JSON_NUM, JSON_NULL, JSON_STR, JSON_OBJ, JSON_BOOL, JSON_ARRAY } JSON_TYPE;

typedef enum { JSON_PARSE_OK, JSON_PARSE_EXPECT_VALUE, JSON_PARSE_INVALID_VALUE, JSON_PARSE_ROOT_NOT_SINGULAR } JSON_RESULT;

typedef struct {
  JSON_TYPE type;
} bscl_json_base_t;

typedef struct {
  bscl_list_head_t head;
  char *key;
  bscl_json_base_t *value;
} bscl_json_pair_t;

typedef struct {
  bscl_json_base_t base;
  double value;
} bscl_json_num_t;
typedef struct {
  bscl_json_base_t base;
  bool value;
} bscl_json_bool_t;

typedef struct {
  bscl_json_base_t base;
  char *value;
} bscl_json_string_t;

typedef struct {
  bscl_json_base_t base;
  /*object contensts*/
  bscl_list_head_t contents;
} bscl_json_object_t;

typedef struct {
  bscl_json_base_t base;
  /*array contents*/
  bscl_list_head_t contents;
} bscl_json_array_t;

typedef struct {
  const char *str;
  const char *curr;
  size_t size;
} bscl_json_parser_t;

int bscl_json_parse(bscl_json_base_t **base, bscl_json_parser_t *parser);
#define bscl_json_type(v) (((bscl_json_base_t *)v)->type)
#define bscl_json_value(b) (b->value)
