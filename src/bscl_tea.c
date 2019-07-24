#include <stdlib.h>

#include "bscl_tea.h"
static uint8_t bscl_tea_key[16] = {0x29, 0x23, 0xbe, 0x84, 0xe1, 0x6c, 0xd6, 0xae,
                                   0x52, 0x90, 0x49, 0xf1, 0xf1, 0xbb, 0xe9, 0xeb};

void bscl_xor_64(uint8_t *data, uint8_t *para) {
  int i = 0;
  for (i = 0; i < 8; i++)
    data[i] = data[i] ^ para[i];
}

void bscl_encrypt(uint32_t *v, uint32_t *k) {
  uint32_t v0 = v[0], v1 = v[1], sum = 0, i;           /* set up */
  uint32_t delta = 0x9e3779b9;                         /* a key schedule constant */
  uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3]; /* cache key */
  for (i = 0; i < 32; i++) {                           /* basic cycle start */
    sum += delta;
    v0 += ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
    v1 += ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
  } /* end cycle */
  v[0] = v0;
  v[1] = v1;
}

void bscl_decrypt(uint32_t *v, uint32_t *k) {
  uint32_t v0 = v[0], v1 = v[1], sum = 0xC6EF3720, i;  /* set up */
  uint32_t delta = 0x9e3779b9;                         /* a key schedule constant */
  uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3]; /* cache key */
  for (i = 0; i < 32; i++) {                           /* basic cycle start */
    v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
    v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
    sum -= delta;
  } /* end cycle */
  v[0] = v0;
  v[1] = v1;
}

bool bscl_encryptData(uint8_t *data, uint32_t len) {
  int i = 0;
  int n = 0;
  if (len % 8)
    return false;

  n = len / 8;
  bscl_encrypt((uint32_t *)(data + 8 * (n - 1)), (uint32_t *)bscl_tea_key);
  for (i = n - 2; i >= 0; i--) {
    bscl_xor_64(data + 8 * i, data + 8 * (i + 1));
    bscl_encrypt((uint32_t *)(data + 8 * i), (uint32_t *)bscl_tea_key);
  }
  return true;
}

bool bscl_decryptData(uint8_t *data, uint32_t len) {
  int i = 0;
  int n = 0;
  if (len % 8) {
    return false;
  }

  n = len / 8;

  for (i = 0; i < n - 1; i++) {
    bscl_decrypt((uint32_t *)(data + 8 * i), (uint32_t *)bscl_tea_key);
    bscl_xor_64(data + 8 * i, data + 8 * (i + 1));
  }
  bscl_decrypt((uint32_t *)(data + 8 * (n - 1)), (uint32_t *)bscl_tea_key);
  return true;
}


