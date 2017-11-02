#ifndef _INCLUDE_PLATFORM_IODEVICE_H_
#define _INCLUDE_PLATFORM_IODEVICE_H_
#include "common/platform_common.h"
#include <assert.h>
#include <semaphore.h>
#include <stdint.h>

struct platform_ioDevice_t_;

typedef struct {
  size_t (*read)(struct platform_iodevice_t_ *, void *, size_t);
  size_t (*write)(struct platform_iodevice_t_ *, void *, size_t);
  void (*close)(struct platform_iodevice_t_ *);
} platform_iodevice_vtable_t;
typedef struct platform_iodevice_t_ {
  const platform_iodevice_vtable_t *vptr;
} platform_iodevice_t;

#define platform_iodevice_read(dev, buf, len) dev->vptr->read(dev, buf, len)
#define platform_iodevice_write(dev, buf, len) dev->vptr->write(dev, buf, len)
#define platform_iodevice_close(dev) dev->vptr->close()

#endif /*_INCLUDE_PLATFORM_IODEVICE_H_*/
