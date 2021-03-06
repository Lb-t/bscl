#ifndef INCLDUE_PALTFORM_TEST_H_
#define INCLDUE_PALTFORM_TEST_H_
#include <stdio.h>
static int bscl_test_main_ret = 0;
static int bscl_test_count = 0;
static int bscl_test_pass = 0;

#define PALTFORM_EXPECT_EQ_BASE(equality, expect, actual, format)              \
  \
do {                                                                           \
    bscl_test_count++;                                                     \
    if (equality)                                                              \
      bscl_test_pass++;                                                    \
    else {                                                                     \
      fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n",        \
              __FILE__, __LINE__, expect, actual);                             \
      bscl_test_main_ret = 1;                                              \
    }                                                                          \
  \
}                                                                         \
  while (0)

#define PALTFORM_EXPECT_EQ_INT(expect, actual)                                 \
  \
PALTFORM_EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")

#endif
