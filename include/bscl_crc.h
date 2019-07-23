#pragma once
#include "bscl_common.h"
uint16_t bscl_crc16_ccitt(uint8_t *q, int len);

uint32_t bscl_crc32(const void*data,size_t len,uint32_t init);
