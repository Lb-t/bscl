/*
 * Bin's utility library
 */
#ifndef _INCLUDE_PLATFORM_NETWORK_H_
#define _INCLUDE_PLATFORM_NETWORK_H_
#include "io/platform_IODevice.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef struct platform_udpBroadcaster_t_ platform_udpBroadcaster_t;
bool platform_udpBroadcaster_init(platform_udpBroadcaster_t *this,
                                  uint16_t local_port, uint16_t remote_port);
platform_udpBroadcaster_t *platform_udpBroadcaster_new(uint16_t local_port,
                                                       uint16_t remote_port);
void platform_udpBroadcaster_delete(platform_udpBroadcaster_t *this);
size_t platform_udpBroadcaster_write(platform_udpBroadcaster_t *this,
                                     void *data, size_t len);
size_t platform_udpBroadcaster_read(platform_udpBroadcaster_t *this, void *data,
                                    size_t len);

typedef struct platform_tcpClient_t_ platform_tcpClient_t;
platform_tcpClient_t *platform_tcpClient_new(uint16_t port,
                                             unsigned long server_ip,
                                             uint16_t server_port);
bool platform_tcpClient_init(platform_tcpClient_t *this, uint16_t port,
                             unsigned long server_ip, uint16_t server_port);
size_t platform_tcpClient_read(platform_tcpClient_t *this, void *buf,
                               size_t len);
size_t platform_tcpClient_write(platform_tcpClient_t *this, void *buf,
                                size_t len);

typedef void (*platform_tcpServerCallback_t)(platform_tcpClient_t *client,
                                             void *buf, size_t len);

typedef struct platform_tcpServer_t_ platform_tcpServer_t;
platform_tcpServer_t *
platform_tcpServer_new(uint16_t port, platform_tcpServerCallback_t callback);
bool platform_tcpServer_init(platform_tcpServer_t *this, uint16_t port,
                             platform_tcpServerCallback_t callback);

#endif /*!_INCLUDE_PLATFORM_NETWORK_H_*/
