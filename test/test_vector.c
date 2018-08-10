//#include "cmockery/cmockery.h"
#include "platform_vector.h"


// A test case that does nothing and succeeds.
void test_vector1(void **state) {
  static unsigned long temp[1000] = {5};
  platform_vector_t *vector =
      platform_vector_new(0, temp, sizeof(unsigned long));
  for (int i = 0; i < 10000; ++i) {
    ++temp[0];
    // printf("insert i=%d\n", i);
    platform_vector_insert(vector, platform_vector_size(vector),
                           temp[0] % 20 + 1, temp);
  }
  while (platform_vector_size(vector)) {
    // printf("remove size=%d\n", platform_vector_size(vector));
    platform_vector_remove(vector, 0, 1);
  }

  for (int i = 0; i < 10000; ++i) {
    // printf("insert i=%d\n", i);
    --temp[0];
    platform_vector_insert(vector, platform_vector_size(vector),
                           temp[0] % 20 + 1, temp);
  }
  while (platform_vector_size(vector)) {
    // printf("remove size=%d\n", platform_vector_size(vector));
    platform_vector_remove(vector, 0, 1);
  }
  // printf("size:%d capacity:%d\n", platform_vector_size(vector),
  //      platform_vector_capacity(vector));
  platform_vector_delete(vector);
}

void test_vector2(void **state) {
  int temp = 5;
  platform_vector_t *vector = platform_vector_new(-1, 0, sizeof(int));
  // platform_vector_delete(vector);
  vector = platform_vector_new(0, 0, sizeof(int));
  platform_vector_delete(0);
}

int main(int argc, char *argv[]) {
  printf("startup \n");
  test_vector1(0);
  // const UnitTest tests[] = {
  //     unit_test(test_vector1),
  // };
  // return run_tests(tests);
}
