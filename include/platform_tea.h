#ifndef INCLUDE_PLATFORM_TEA_H_
#define INCLUDE_PLATFORM_TEA_H_
#include "platform_common.h"

bool platform_encryptData(uint8_t *data, uint32_t len);
bool platform_decryptData(uint8_t *data, uint32_t len);
void platform_fillRandom(uint8_t *data, uint16_t len);

#endif
