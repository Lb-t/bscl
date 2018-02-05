#ifndef INCLUDE_PLATFORM_JSON_H_
#define INCLUDE_PLATFORM_JSON_H_
#include <stdbool.h>
#include <stdlib.h>

typedef enum {
  PLATFORM_JSON_NUM,
  PLATFORM_JSON_NULL,
  PLATFORM_JSON_STR,
  PLATFORM_JSON_OBJ,
  PLATFORM_JSON_BOOL,
  PLATFORM_JSON_ARRAY
} PLATFORM_JSON_TYPE;

typedef enum {
  PLATFORM_JSON_PARSE_OK,
  PLATFORM_JSON_PARSE_EXPECT_VALUE,
  PLATFORM_JSON_PARSE_INVALID_VALUE,
  PLATFORM_JSON_PARSE_ROOT_NOT_SINGULAR
};

typedef struct { PLATFORM_JSON_TYPE type; } platform_json_base_t;

typedef struct {
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
  /*object contensts*/
  platform_json_pair_t *pair;
  size_t count;
} platform_json_object_t;

typedef struct {
  platform_json_base_t base;
  /*array contents*/
  platform_json_base_t *values;
  size_t count;
} platform_json_array_t;

typedef struct {
  const char *str;
  const char *curr;
  size_t size;
} platform_json_parser_t;

int platform_json_parse(platform_json_base_t **base, const char *in);
#define platform_json_type(v) (((platform_json_base_t *)v)->type)
#define platform_json_value(b) (b->value)
#endif
