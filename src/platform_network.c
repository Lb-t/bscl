#include "platform_network.h"
#include "platform_vector.h"
#include <limits.h>
#include <stdlib.h>
#include <time.h>
static uint32_t platform_network_ip;
void platform_network_close_socket(int fd) {
#ifdef _WIN32
  shutdown(fd, SD_BOTH);
  closesocket(fd);
#else
  shutdown(fd, SHUT_RDWR);
  close(fd);
#endif
}
void platform_network_get_local_ip(void) {
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = 0;
  addr.sin_addr.s_addr = INADDR_ANY;
  int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (fd < 0) {
#ifndef NODEBUG
    perror("socket failed.");
#endif
  }
  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
#ifndef NODEBUG
    fprintf(stderr, "bind error.\n");
#endif
  }

#ifdef _WIN32
  char
#else
  int
#endif
      bOpt = true;
  setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &bOpt, sizeof(bOpt));

  int fromlen = sizeof(addr);
  if (getsockname(fd, (struct sockaddr *)&addr, &fromlen)) {
#ifndef NODEBUG
    fprintf(stderr, "getsockname failed.\n");
#endif
    return;
  }

/*no block mode*/
#ifdef _WIN32
  unsigned long argp = 1;
  ioctlsocket(fd, FIONBIO, &argp);
#else
  ioctl(fd, FIONBIO, 1);
#endif

#if (RAND_MAX > 0xffff)
#define num_type uint32_t
#elif (RAND_MAX <= 0xffff) && (RAND_MAX > 0xff)
#define num_type uint16_t
#else
#define num_type uint8_t
#endif
  num_type *sendbuf = (num_type *)malloc(1024);
  if (!sendbuf) {
#ifndef NODEBUG
    fprintf(stderr, "malloc error.\n");
#endif
    return;
  }
  num_type *recvbuf = (num_type *)malloc(1024);
  if (!recvbuf) {
#ifndef NODEBUG
    fprintf(stderr, "malloc error.\n");
#endif
    return;
  }
  // fill the buffer with ramdom number
  srand((unsigned int)time(NULL));
  for (int i = 0; i < 1024 / sizeof(num_type); ++i) {
    sendbuf[i] = rand();
  }

  addr.sin_addr.s_addr = INADDR_BROADCAST;
  sendto(fd, (char *)sendbuf, 1024, 0, (struct sockaddr *)&addr,
         sizeof(struct sockaddr_in));

  while (recvfrom(fd, (char *)recvbuf, 1024, 0, (struct sockaddr *)&addr,
                  &fromlen) > 0) {
    for (int i = 0; i < 1024 / sizeof(num_type); ++i) {
      if (recvbuf[i] != sendbuf[i]) {
        continue;
      }
    }
#ifndef NODEBUG
    printf("ip=%s\n", inet_ntoa(addr.sin_addr));
#endif
    platform_network_ip = addr.sin_addr.s_addr;
  }
  platform_network_close_socket(fd);
  return;
}
#ifdef _WIN32
void platform_netowrk_init(void) {
  WSADATA wsaData;
  static bool isInitialized = false;
  if (!isInitialized) {
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res != 0) {
      perror("WSAStartup failed:\n");
    }
    /*get local ip adress*/
    platform_network_get_local_ip();
    isInitialized = true;
  }
}
#endif

int platform_udp_new(const uint16_t port) {
  platform_netowrk_init();
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = 0;
  int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (fd < 0) {
#ifndef NODEBUG
    perror("socket failed.");
#endif
    return fd;
  }
  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
#ifndef NODEBUG
    fprintf(stderr, "bind error.\n");
#endif
  }

#ifdef _WIN32
  char
#else
  int
#endif
      bOpt = true;
  setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &bOpt, sizeof(bOpt));
  return fd;
}

int platform_tcp_new(const uint16_t port) {
  platform_netowrk_init();
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = 0;
  int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (fd < 0) {
#ifndef NODEBUG
    perror("socket failed.");
#endif
    return fd;
  }
  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
#ifndef NODEBUG
    fprintf(stderr, "bind error.\n");
#endif
  }
  return fd;
}

int platform_udp_receive(const int fd, void *const data, const int len,
                         uint32_t *const ip, uint16_t *const port) {
  struct sockaddr_in addr;
  int fromlen = sizeof(addr);
  int res;
  do {
    res = recvfrom(fd, data, len, 0, (struct sockaddr *)&addr, &fromlen);
  } while (addr.sin_addr.s_addr == platform_network_ip);
  *ip = addr.sin_addr.s_addr;
  *port = ntohs(addr.sin_port);
  return res;
}

int platform_tcp_accept(const int fp, uint32_t *const ip,
                        uint16_t *const port) {
  struct sockaddr_in addr;
  int addrlen = sizeof(struct sockaddr_in);
  int res = accept(fp, (struct sockaddr *)&addr, &addrlen);
  *ip = addr.sin_addr.s_addr;
  *port = ntohs(addr.sin_port);
  return res;
}
