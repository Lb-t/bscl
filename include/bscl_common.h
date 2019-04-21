#pragma once

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
#define bscl_assert(expression)                                            \
  mock_assert((int)(expression), #expression, __FILE__, __LINE__)
#elif !defined(bscl_assert)
#define bscl_assert(p) assert(p)
#endif
#define bscl_abort() abort()
//#define new (CLASS, ...) bscl_##CLASS##_new(__VA_ARGS__)
//#define delete (CLASS, obj) bscl_##CLASS##_delete(obj)
#ifndef offsetof
#define offsetof(s, m) ((size_t) & (((s *)0)->m))
#endif
#ifndef container_of
#define container_of(ptr, type, member)                                        \
  (type *)((char *)ptr - offsetof(type, member))
#endif
#define BSCL_SET_BIT(REG, BIT) ((REG) |= (BIT))
#define BSCL_CLEAR_BIT(REG, BIT) ((REG) &= ~(BIT))
#define BSCL_READ_BIT(REG, BIT) ((REG) & (BIT))
#define BSCL_CHECK_FLAG(TARGET, FLAG)                                      \
  (bscl_READ_BIT(TARGET, FLAG) ? bscl_CLEAR_BIT(REG, BIT), true : false)
#define BSCL_SET_FLAG(TARGET, FLAG) bscl_SET_BIT(REG, BIT)

bool bscl_byteCmp(uint8_t *b1, uint8_t *b2, size_t len);
