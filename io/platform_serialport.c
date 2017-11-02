#include "platform_serialport.h"
size_t platform_serialport_read(platform_iodevice_t *this, void *data,
	size_t len);
size_t platform_serialport_write(platform_iodevice_t *this, void *data,
	size_t len);
void platform_serialport_close(platform_iodevice_t *this);
platform_iodevice_t *platform_serialport_open(char *name);
const platform_iodevice_vtable_t platform_serialport_vtable = {
    .read = platform_serialport_read,
    .write = platform_serialport_write,
    .close = platform_serialport_close,
};

size_t platform_serialport_read(platform_iodevice_t *this, void *data,
                                size_t len) {
  return 0;
}
size_t platform_serialport_write(platform_iodevice_t *this, void *data,
                                 size_t len) {
  return 0;
}
void platform_serialport_close(platform_iodevice_t *this) {}
platform_iodevice_t *platform_serialport_open(char *name) {
}
