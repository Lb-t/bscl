#include "bscl_json.h"
#include "bscl_test.h"

static void test_parse_null() {
  bscl_json_base_t *v = NULL;
  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_PARSE_OK,
                         bscl_json_parse(&v, "null"));
  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_NULL, bscl_json_type(v));

  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_PARSE_OK,
                         bscl_json_parse(&v, " null"));
  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_NULL, bscl_json_type(v));

  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_PARSE_OK,
                         bscl_json_parse(&v, "null "));
  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_NULL, bscl_json_type(v));

  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_PARSE_INVALID_VALUE,
                         bscl_json_parse(&v, "n ull "));
  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_PARSE_INVALID_VALUE,
                         bscl_json_parse(&v, "aull "));
  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_PARSE_INVALID_VALUE,
                         bscl_json_parse(&v, " nul"));
  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_PARSE_INVALID_VALUE,
                         bscl_json_parse(&v, "nu"));
}

static void test_parse_bool() {
  bscl_json_base_t *v = NULL;
  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_PARSE_OK,
                         bscl_json_parse(&v, "true"));
  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_BOOL, bscl_json_type(v));
  PALTFORM_EXPECT_EQ_INT(true,
                         bscl_json_value(((bscl_json_bool_t *)v)));

  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_PARSE_OK,
                         bscl_json_parse(&v, "  true "));
  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_BOOL, bscl_json_type(v));
  PALTFORM_EXPECT_EQ_INT(true,
                         bscl_json_value(((bscl_json_bool_t *)v)));

  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_PARSE_OK,
                         bscl_json_parse(&v, "false"));
  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_BOOL, bscl_json_type(v));
  PALTFORM_EXPECT_EQ_INT(false,
                         bscl_json_value(((bscl_json_bool_t *)v)));

  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_PARSE_OK,
                         bscl_json_parse(&v, "  false "));
  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_BOOL, bscl_json_type(v));
  PALTFORM_EXPECT_EQ_INT(false,
                         bscl_json_value(((bscl_json_bool_t *)v)));

  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_PARSE_INVALID_VALUE,
                         bscl_json_parse(&v, "TRUE"));
  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_PARSE_INVALID_VALUE,
                         bscl_json_parse(&v, "FALSE "));
  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_PARSE_INVALID_VALUE,
                         bscl_json_parse(&v, " atrue"));
  PALTFORM_EXPECT_EQ_INT(BSCL_JSON_PARSE_INVALID_VALUE,
                         bscl_json_parse(&v, "nfalse"));
}

/* ... */

static void test_parse() {
  test_parse_null();
  test_parse_bool();
  /* ... */
}

int main() {
  test_parse();
  printf("%d/%d (%3.2f%%) passed\n", bscl_test_pass, bscl_test_count,
         bscl_test_pass * 100.0 / bscl_test_count);
  return bscl_test_main_ret;
}
