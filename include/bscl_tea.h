#pragma once
#include "bscl_common.h"

bool bscl_encryptData(uint8_t *data, uint32_t len);
bool bscl_decryptData(uint8_t *data, uint32_t len);
void bscl_fillRandom(uint8_t *data, uint16_t len);

