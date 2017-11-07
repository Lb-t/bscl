#ifndef _INCLUDE_PLATFORM_AUDIO_H_
#define _INCLUDE_PLATFORM_AUDIO_H_
#include <stdbool.h>
#include "platform_iodevice.h"

platform_IODevice_t* platform_audioIO_init(uint16_t sampe_rate, uint16_t frame_size);
size_t platform_audio_write(platform_IODevice_t* dev, void *data, size_t len);
#endif /*!_INCLUDE_PORT_AUDIO_H_*/
