#pragma once
#include <stdint.h>

typedef struct {
  unsigned int state[4];    /* state (ABCD) */
  unsigned int count[2];    /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64]; /* input buffer */
} bscl_md5_context_t;

typedef struct {
  uint8_t digest[16];
}bscl_md5_digest_t;

void bscl_md5_prepare(bscl_md5_context_t *ctx);
void bscl_md5_caculate_next(bscl_md5_context_t *ctx, const uint8_t *data, unsigned int len);
void bscl_md5_finish(bscl_md5_context_t *ctx,bscl_md5_digest_t *digest);
