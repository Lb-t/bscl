#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#endif

#ifdef _WIN32
void bscl_netowrk_init(void);
#else
#define bscl_netowrk_init()
#endif

int bscl_udp_new(const uint16_t port);
int bscl_udp_receive(const int fd, void *const data, const int len, uint32_t *const ip, uint16_t *const port);
static inline int bscl_udp_broadcast(const int fd, void const *const data, const int len, const uint16_t port) {
  struct sockaddr_in addr = {.sin_family = AF_INET, .sin_addr.s_addr = INADDR_BROADCAST};
  addr.sin_port = htons(port);
  return sendto(fd, data, len, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
}

static inline int bscl_udp_unicast(const int fd, void const *const data, const int len, const uint32_t ip,
                                   const uint16_t port) {
  struct sockaddr_in addr = {.sin_family = AF_INET, .sin_addr.s_addr = ip};
  addr.sin_port = htons(port);
  return sendto(fd, data, len, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
}

int bscl_tcp_new(const uint16_t port);
static inline int bscl_tcp_connect(const int fd, const uint32_t ip, const uint16_t port) {
  struct sockaddr_in addr;
  // connect to server
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = ip;
  return connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
}
#define bscl_tcp_listen(fd) listen(fd, SOMAXCONN)
#define bscl_tcp_read(fd, data, len) recv(fd, data, len, 0)
#define bscl_tcp_write(fd, data, len) send(fd, data, len, 0)
int bscl_tcp_accept(const int fp, uint32_t *const ip, uint16_t *const port);
int bscl_tcp_delete(int fd);
