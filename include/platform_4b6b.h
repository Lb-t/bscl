#ifndef INCLUDE_PLATFORM_4B6B_H_
#define INCLUDE_PLATFORM_4B6B_H_
#include "stdio.h"
#include "stdint.h"
void platform_4b6b_encoder(uint16_t *indata, uint8_t *outdata,
		uint16_t inlen_16, uint16_t outlen_8);
int platform_4b6b_decoder(uint8_t *indata, uint16_t *outdata, uint16_t inlen_8,
		uint16_t outlen_16);
#endif
