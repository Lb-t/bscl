#include "platform_json.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
void platform_json_parse_ws(platform_json_parser_t *parser) {
  while (*parser->curr == ' ' || *parser->curr == '\t' ||
         *parser->curr == '\n' || *parser->curr == '\r')
    ++parser->curr;
}

int platform_json_parse_null(platform_json_base_t **b,
                             platform_json_parser_t *parser) {
  assert(parser);

  platform_json_parse_ws(parser);
  // static int i = 0;
  if ((parser->curr + 4) > (parser->str + parser->size)) {
    // printf("size error %x %x %d %s\n", parser->curr, parser->str,
    // parser->size,
    //      parser->curr);
    return PLATFORM_JSON_PARSE_INVALID_VALUE;
  }

  if (parser->curr[0] == 'n' && parser->curr[1] == 'u' &&
      parser->curr[2] == 'l' && parser->curr[3] == 'l') {
    parser->curr += 4;
    if (!(*b))
      *b = (platform_json_base_t *)malloc(sizeof(platform_json_base_t));
    (*b)->type = PLATFORM_JSON_NULL;

    return PLATFORM_JSON_PARSE_OK;
  } else {
    // printf("invalid %s\n", parser->curr);
    return PLATFORM_JSON_PARSE_INVALID_VALUE;
  }
}

int platform_json_parse_bool(platform_json_base_t **b,
                             platform_json_parser_t *parser) {
  assert(parser);
  platform_json_parse_ws(parser);
  if (((parser->curr + 4) <= (parser->str + parser->size)) &&
      parser->curr[0] == 't' && parser->curr[1] == 'r' &&
      parser->curr[2] == 'u' && parser->curr[3] == 'e') {
    parser->curr += 4;
    if (!(*b)) {
      *b = (platform_json_base_t *)malloc(sizeof(platform_json_bool_t));
    }
    (*b)->type = PLATFORM_JSON_BOOL;
    printf("json true\n");
    ((platform_json_bool_t *)(*b))->value = true;
  } else if ((parser->curr + 5) <= (parser->str + parser->size) &&
             parser->curr[0] == 'f' && parser->curr[1] == 'a' &&
             parser->curr[2] == 'l' && parser->curr[3] == 's' &&
             parser->curr[4] == 'e') {
    parser->curr += 5;
    if (!(*b)) {
      *b = (platform_json_base_t *)malloc(sizeof(platform_json_bool_t));
    }
    (*b)->type = PLATFORM_JSON_BOOL;
    printf("json false\n");
    ((platform_json_bool_t *)(*b))->value = false;

  } else {
    // printf("invalid %x %x %d %s\n", parser->curr, parser->str, parser->size,
    //       parser->curr);
    return PLATFORM_JSON_PARSE_INVALID_VALUE;
  }

  return PLATFORM_JSON_PARSE_OK;
}

int platform_json_parse_num(platform_json_base_t **b,
                            platform_json_parser_t *parser) {
  assert(parser);
  platform_json_parse_ws(parser);
  int d;
  while (parser->curr <= (parser->str + parser->size)) {
    if (*parser->curr == '0') {
    } else if ((*parser->curr > '0') && (*parser->curr <= '9')) {
    } else if (*parser->curr == '-') {
    } else if (*parser->curr == 'e' || *parser->curr == 'E') {
    } else {
    }
    ++parser->curr;
  }
  
}

int platform_json_parse_object() {}

int platform_json_parse(platform_json_base_t **base, const char *in) {
  assert(in);
  platform_json_parser_t parser;
  parser.curr = in;
  parser.str = in;
  parser.size = strlen(in);
  platform_json_parse_ws(&parser);
  switch (*parser.curr) {
  case 'n':
    return platform_json_parse_null(base, &parser);
  case 'f':
    return platform_json_parse_bool(base, &parser);
  case 't':
    return platform_json_parse_bool(base, &parser);
  case '\0':
    return PLATFORM_JSON_PARSE_EXPECT_VALUE;
  default:
    return PLATFORM_JSON_PARSE_INVALID_VALUE;
  }
}