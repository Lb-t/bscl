#include "bscl_json.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

static void bscl_json_parse_ws(bscl_json_parser_t *parser) {
  while (*parser->curr == ' ' || *parser->curr == '\t' || *parser->curr == '\n' || *parser->curr == '\r')
    ++parser->curr;
}

static int bscl_json_parse_null(bscl_json_base_t **b, bscl_json_parser_t *parser) {
  assert(parser);

  bscl_json_parse_ws(parser);
  // static int i = 0;
  if ((parser->curr + 4) > (parser->str + parser->size)) {
    // printf("size error %x %x %d %s\n", parser->curr, parser->str,
    // parser->size,
    //      parser->curr);
    return JSON_PARSE_INVALID_VALUE;
  }

  if (parser->curr[0] == 'n' && parser->curr[1] == 'u' && parser->curr[2] == 'l' && parser->curr[3] == 'l') {
    parser->curr += 4;
    if (!(*b))
      *b = (bscl_json_base_t *)malloc(sizeof(bscl_json_base_t));
    (*b)->type = JSON_NULL;

    return JSON_PARSE_OK;
  } else {
    // printf("invalid %s\n", parser->curr);
    return JSON_PARSE_INVALID_VALUE;
  }
}

static int bscl_json_parse_bool(bscl_json_base_t **b, bscl_json_parser_t *parser) {
  assert(parser);
  bscl_json_parse_ws(parser);
  if (((parser->curr + 4) <= (parser->str + parser->size)) && parser->curr[0] == 't' && parser->curr[1] == 'r' &&
      parser->curr[2] == 'u' && parser->curr[3] == 'e') {
    parser->curr += 4;
    if (!(*b)) {
      *b = (bscl_json_base_t *)malloc(sizeof(bscl_json_bool_t));
    }
    (*b)->type = JSON_BOOL;
    printf(" json true\n");
    ((bscl_json_bool_t *)(*b))->value = true;
  } else if ((parser->curr + 5) <= (parser->str + parser->size) && parser->curr[0] == 'f' && parser->curr[1] == 'a' &&
             parser->curr[2] == 'l' && parser->curr[3] == 's' && parser->curr[4] == 'e') {
    parser->curr += 5;
    if (!(*b)) {
      *b = (bscl_json_base_t *)malloc(sizeof(bscl_json_bool_t));
    }
    (*b)->type = JSON_BOOL;
    printf(" json false\n");
    ((bscl_json_bool_t *)(*b))->value = false;
  } else {
    // printf("invalid %x %x %d %s\n", parser->curr, parser->str, parser->size,
    //       parser->curr);
    return JSON_PARSE_INVALID_VALUE;
  }

  return JSON_PARSE_OK;
}

static int bscl_json_parse_num(bscl_json_base_t **b, bscl_json_parser_t *parser) {
  assert(parser);
  bscl_json_parse_ws(parser);
  char *endptr;
  double n = strtod(parser->curr, &endptr);
  if (errno != 0) {
    return JSON_PARSE_INVALID_VALUE;
  }
  bscl_json_num_t *num = (bscl_json_num_t *)malloc(sizeof(bscl_json_num_t));
  num->base.type = JSON_NUM;
  num->value = n;
  parser->curr = endptr;
  return JSON_PARSE_OK;
}

static int bscl_json_parse_str(bscl_json_base_t **b, bscl_json_parser_t *parser) {
  if (*parser->curr++ != '\"') {
    return JSON_PARSE_INVALID_VALUE;
  }
  bscl_json_string_t *str = (bscl_json_string_t *)malloc(sizeof(bscl_json_string_t));
  const char *str_begin = parser->curr;
  while (parser->curr < parser->str + parser->size) {
    if (*parser->curr == '\"') {
      int str_len = parser->curr - str_begin;
      if (str_len < 1) {
        free(str);
        return JSON_PARSE_INVALID_VALUE;
      }
      str->value = (char *)malloc(str_len + 1);
      memcpy(str->value, str_begin, str_len);
      str->value[str_len] = 0;
      ++parser->curr;
      return JSON_PARSE_OK;
    }
    ++parser->curr;
  }
  return JSON_PARSE_INVALID_VALUE;
}

typedef enum { PARSE_WAIT_KEY_START, PARSE_WAIT_KEY_END, PARSE_WAIT_VALUE, PARSE_WAIT_END, PARSE_ERROR } parse_state_t;

static int bscl_json_parse_object(bscl_json_base_t **b, bscl_json_parser_t *parser) {
  if (*parser->curr++ != '{') {
    return JSON_PARSE_INVALID_VALUE;
  }
  bscl_json_object_t *obj = (bscl_json_object_t *)malloc(sizeof(bscl_json_object_t));
  parse_state_t state = PARSE_WAIT_KEY_START;
  obj->base.type = JSON_OBJ;
  list_init(&obj->contents);

  const char *key_begin = NULL;
  bscl_json_pair_t *pair;
  while (parser->curr < parser->str + parser->size) {
    bscl_json_parse_ws(parser);
    switch (state) {
    case PARSE_WAIT_KEY_START:
      if (*parser->curr == '\"') {
        key_begin = parser->curr + 1;
        pair = (bscl_json_pair_t *)malloc(sizeof(bscl_json_pair_t));
        pair->key = NULL;
        pair->value = NULL;
        state = PARSE_WAIT_KEY_END;
      } else {
        free(obj);
        return JSON_PARSE_INVALID_VALUE;
      }
      break;
    case PARSE_WAIT_KEY_END:
      if (*parser->curr == '\"') {
        // key end
        int key_len = parser->curr - key_begin;
        if (key_len < 1) {
          // invalid key string
          free(obj);
          free(pair);
          return JSON_PARSE_INVALID_VALUE;
        }
        pair->key = (char *)malloc(key_len + 1);
        memcpy(pair->key, key_begin, key_len);
        pair->key[key_len] = 0;
        state = PARSE_WAIT_VALUE;
      }
      break;
    case PARSE_WAIT_VALUE:
      if (*parser->curr == ':') {
        ++parser->curr;
        int res = bscl_json_parse(&pair->value, parser);
        if (res != JSON_PARSE_OK) {
          free(obj);
          free(pair);
          return res;
        }

        list_insert_tail(&obj->contents, &pair->head);
        state = PARSE_WAIT_END;
      } else {
        // error
        free(obj);
        free(pair);
        return JSON_PARSE_INVALID_VALUE;
      }
      break;
    case PARSE_WAIT_END:
      if (*parser->curr == '}') {
        ++parser->curr;
        *b = (bscl_json_base_t *)obj;
        return JSON_PARSE_OK;
      } else if (*parser->curr == ',') {
        pair = NULL;
        state = PARSE_WAIT_KEY_START;
      }
      break;
    default:
      break;
    }
    ++parser->curr;
  }
  return JSON_PARSE_OK;
}

static int bscl_json_parse_array(bscl_json_base_t **b, bscl_json_parser_t *parser) {
  if (*parser->curr++ != '[') {
    return JSON_PARSE_INVALID_VALUE;
  }

  bscl_json_array_t *array = (bscl_json_array_t *)malloc(sizeof(bscl_json_array_t));
  array->base.type = JSON_ARRAY;
  list_init(&array->contents);
  bscl_json_base_t *base;
  while (parser->curr < parser->str + parser->size) {
    int res = bscl_json_parse(&base, parser);
    if (res != JSON_PARSE_OK) {
      free(array);
      return res;
    }
    bscl_json_parse_ws(parser);
    if (*parser->curr == ',') {
    } else if (*parser->curr == ']') {
      *b = (bscl_json_base_t *)array;
      return JSON_PARSE_OK;
    } else {
      free(array);
      return JSON_PARSE_INVALID_VALUE;
    }
    *parser->curr++;
  }
  return JSON_PARSE_INVALID_VALUE;
}

int bscl_json_parse(bscl_json_base_t **base, bscl_json_parser_t *parser) {
  assert(base);
  assert(parser);
  bscl_json_parse_ws(parser);
  switch (*parser->curr) {
  case 'n':
    return bscl_json_parse_null(base, parser);
  case 'f':
    return bscl_json_parse_bool(base, parser);
  case 't':
    return bscl_json_parse_bool(base, parser);
  case '{':
    return bscl_json_parse_object(base, parser);
  case '[':
    return bscl_json_parse_array(base, parser);
  case '\"':
    return bscl_json_parse_str(base, parser);
  case '\0':
    return JSON_PARSE_EXPECT_VALUE;
  default:
    return bscl_json_parse_num(base, parser);
  }
}
