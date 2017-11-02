#ifndef INCLUDE_PLATFORM_SERIALPORT_H_
#define INCLUDE_PLATFORM_SERIALPORT_H_
#include "io/platform_iodevice.h"

typedef struct {
  platform_iodevice_t iodevice;
  int count;
} platform_serialport_t;

#define PLATFORM_SERIALPORT(p) container_of(p, platform_serialport_t, iodevice)
#endif
