#include "platform_common.h"

bool platform_byteCmp(uint8_t *b1, uint8_t *b2, size_t len) {
	unsigned int i;
	for (i = 0; i < len; i++)
		if (b1[i] != b2[i])
			return false;
	return true;
}
