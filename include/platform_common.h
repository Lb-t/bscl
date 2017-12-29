#ifndef INCLUDE_PLATFORM_COMMON_H_
#define INCLUDE_PLATFORM_COMMON_H_

#include <assert.h>
#include <limits.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <longjmp.h>
#if UNIT_TESTING
#define platform_assert(expression)                                            \
  mock_assert((int)(expression), #expression, __FILE__, __LINE__)
#elif !defined(platform_assert)
#define platform_assert(p) assert(p)
#endif
#define platform_abort() abort()
//#define new (CLASS, ...) platform_##CLASS##_new(__VA_ARGS__)
//#define delete (CLASS, obj) platform_##CLASS##_delete(obj)
#ifndef offsetof
#define offsetof(s, m) ((size_t) & (((s *)0)->m))
#endif
#ifndef container_of
#define container_of(ptr, type, member)                                        \
  (type *)((char *)ptr - offsetof(type, member))
#endif
#define PLATFORM_SET_BIT(REG, BIT) ((REG) |= (BIT))
#define PLATFORM_CLEAR_BIT(REG, BIT) ((REG) &= ~(BIT))
#define PLATFORM_READ_BIT(REG, BIT) ((REG) & (BIT))
#define PLATFORM_CHECK_FLAG(TARGET, FLAG)                                      \
  (platform_READ_BIT(TARGET, FLAG) ? platform_CLEAR_BIT(REG, BIT), true : false)
#define PLATFORM_SET_FLAG(TARGET, FLAG) platform_SET_BIT(REG, BIT)

bool platform_byteCmp(uint8_t *b1, uint8_t *b2, size_t len);
#endif /* !_INCLUDE_PLATFORM_COMMON_H_ */
