#pragma once
#include "list_head.h"
#include <stdbool.h>
#include <stdlib.h>
typedef enum { JSON_NUM, JSON_NULL, JSON_STR, JSON_OBJ, JSON_BOOL, JSON_ARRAY } JSON_TYPE;

typedef enum { JSON_PARSE_OK, JSON_PARSE_EXPECT_VALUE, JSON_PARSE_INVALID_VALUE, JSON_PARSE_ROOT_NOT_SINGULAR } JSON_RESULT;

typedef struct {
  JSON_TYPE type;
} platform_json_base_t;

typedef struct {
  list_head_t head;
  char *key;
  platform_json_base_t *value;
} platform_json_pair_t;

typedef struct {
  platform_json_base_t base;
  double value;
} platform_json_num_t;
typedef struct {
  platform_json_base_t base;
  bool value;
} platform_json_bool_t;

typedef struct {
  platform_json_base_t base;
  char *value;
} platform_json_string_t;

typedef struct {
  platform_json_base_t base;
  /*object contensts*/
  list_head_t contents;
} platform_json_object_t;

typedef struct {
  platform_json_base_t base;
  /*array contents*/
  list_head_t contents;
} platform_json_array_t;

typedef struct {
  const char *str;
  const char *curr;
  size_t size;
} platform_json_parser_t;

int platform_json_parse(platform_json_base_t **base, platform_json_parser_t *parser);
#define platform_json_type(v) (((platform_json_base_t *)v)->type)
#define platform_json_value(b) (b->value)
