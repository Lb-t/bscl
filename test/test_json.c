#include "platform_json.h"
#include "platform_test.h"

static void test_parse_null() {
  platform_json_base_t *v = NULL;
  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_PARSE_OK,
                         platform_json_parse(&v, "null"));
  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_NULL, platform_json_type(v));

  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_PARSE_OK,
                         platform_json_parse(&v, " null"));
  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_NULL, platform_json_type(v));

  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_PARSE_OK,
                         platform_json_parse(&v, "null "));
  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_NULL, platform_json_type(v));

  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_PARSE_INVALID_VALUE,
                         platform_json_parse(&v, "n ull "));
  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_PARSE_INVALID_VALUE,
                         platform_json_parse(&v, "aull "));
  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_PARSE_INVALID_VALUE,
                         platform_json_parse(&v, " nul"));
  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_PARSE_INVALID_VALUE,
                         platform_json_parse(&v, "nu"));
}

static void test_parse_bool() {
  platform_json_base_t *v = NULL;
  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_PARSE_OK,
                         platform_json_parse(&v, "true"));
  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_BOOL, platform_json_type(v));
  PALTFORM_EXPECT_EQ_INT(true,
                         platform_json_value(((platform_json_bool_t *)v)));

  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_PARSE_OK,
                         platform_json_parse(&v, "  true "));
  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_BOOL, platform_json_type(v));
  PALTFORM_EXPECT_EQ_INT(true,
                         platform_json_value(((platform_json_bool_t *)v)));

  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_PARSE_OK,
                         platform_json_parse(&v, "false"));
  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_BOOL, platform_json_type(v));
  PALTFORM_EXPECT_EQ_INT(false,
                         platform_json_value(((platform_json_bool_t *)v)));

  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_PARSE_OK,
                         platform_json_parse(&v, "  false "));
  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_BOOL, platform_json_type(v));
  PALTFORM_EXPECT_EQ_INT(false,
                         platform_json_value(((platform_json_bool_t *)v)));

  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_PARSE_INVALID_VALUE,
                         platform_json_parse(&v, "TRUE"));
  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_PARSE_INVALID_VALUE,
                         platform_json_parse(&v, "FALSE "));
  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_PARSE_INVALID_VALUE,
                         platform_json_parse(&v, " atrue"));
  PALTFORM_EXPECT_EQ_INT(PLATFORM_JSON_PARSE_INVALID_VALUE,
                         platform_json_parse(&v, "nfalse"));
}

/* ... */

static void test_parse() {
  test_parse_null();
  test_parse_bool();
  /* ... */
}

int main() {
  test_parse();
  printf("%d/%d (%3.2f%%) passed\n", platform_test_pass, platform_test_count,
         platform_test_pass * 100.0 / platform_test_count);
  return platform_test_main_ret;
}