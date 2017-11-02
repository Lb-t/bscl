#include "platform_4b6b.h"

const static uint8_t platform_4b6b_table[16] = { 0x0E, 0x0D, 0x13, 0x16, 0x15,
		0x23, 0x26, 0x25, 0x19, 0x1A, 0x1C, 0x31, 0x32, 0x29, 0x2A, 0x2C };
static int platform_4b6b_tableScan(uint8_t *in, uint8_t *out) {

	switch (*in) {
	case 0x0E:
		*out = 0;
		break;
	case 0x0D:
		*out = 1;
		break;
	case 0x13:
		*out = 2;
		break;
	case 0x16:
		*out = 3;
		break;
	case 0x15:
		*out = 4;
		break;
	case 0x23:
		*out = 5;
		break;
	case 0x26:
		*out = 6;
		break;
	case 0x25:
		*out = 7;
		break;
	case 0x19:
		*out = 8;
		break;
	case 0x1A:
		*out = 9;
		break;
	case 0x1C:
		*out = 10;
		break;
	case 0x31:
		*out = 11;
		break;
	case 0x32:
		*out = 12;
		break;
	case 0x29:
		*out = 13;
		break;
	case 0x2A:
		*out = 14;
		break;
	case 0x2C:
		*out = 15;
		break;
	default:
		return -1;
		//break;	
	}

	return 0;
}
//4b6b
void platform_4b6b_encoder(uint16_t *indata, uint8_t *outdata,
		uint16_t inlen_16, uint16_t outlen_8) {
	int i, j;
	uint16_t temp[4]; //

	if (outlen_8 >= inlen_16 * 3)
		for (i = 0; i < inlen_16; i++) {
			for (j = 0; j < 4; j++)
				temp[j] = (indata[i] >> (4 * j)) & 0x000F;

			outdata[i * 3] = (platform_4b6b_table[temp[0]] & 0x3F)
					+ ((platform_4b6b_table[temp[1]] << 6) & 0xC0); //
			outdata[i * 3 + 1] = ((platform_4b6b_table[temp[1]] >> 2) & 0x0F)
					+ ((platform_4b6b_table[temp[2]] << 4) & 0xF0); //
			outdata[i * 3 + 2] = ((platform_4b6b_table[temp[2]] >> 4) & 0x03)
					+ ((platform_4b6b_table[temp[3]] << 2) & 0xFC); //
		}
}

int platform_4b6b_decoder(uint8_t *indata, uint16_t *outdata, uint16_t inlen_8,
		uint16_t outlen_16) {
	int i, j, len;
	uint8_t result = 0;
	uint8_t temp6[4]; //
	uint8_t temp4[4]; //
	if (((inlen_8 % 3) == 0) && (outlen_16 >= (inlen_8 / 3))) {
		len = inlen_8 / 3;
		for (i = 0; i < len; i++) {
			outdata[i] = 0;

			temp6[0] = indata[3 * i] & 0x3F;
			temp6[1] = ((indata[3 * i] >> 6) & 0x03)
					+ ((indata[3 * i + 1] << 2) & 0x3C);
			temp6[2] = ((indata[3 * i + 1] >> 4) & 0x0F)
					+ ((indata[3 * i + 2] << 4) & 0x30);
			temp6[3] = (indata[3 * i + 2] >> 2) & 0x3F;
			for (j = 0; j < 4; j++) {
				if (platform_4b6b_tableScan(temp6 + j, temp4 + j) != 0) //
					result = 1;
			}

			for (j = 0; j < 4; j++)
				outdata[i] = outdata[i] + (temp4[j] << (j * 4));
		}
		if (result == 0)
			return 0;
	}
	return -1;
}

