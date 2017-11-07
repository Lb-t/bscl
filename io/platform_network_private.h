#ifndef INCLUDE_PLATFORM_NETWORK_PRIVATE_H_
#define INCLUDE_PLATFORM_NETWORK_PRIVATE_H_

#include "include/platform_network.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

//#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SOCKET int
#endif
#include "container/platform_list.h"
#include <stdio.h>

typedef struct { SOCKET sock; } platform_udpBase_t;

struct platform_udpBroadcaster_t_ {
  platform_udpBase_t base;
  struct sockaddr_in broadcast_addr;
  unsigned long local_ip;
};

typedef struct {
  platform_udpBase_t base;
  struct in_addr multiaddr;
  SOCKET sock;
} platform_udpMulticaster_t;

typedef struct {
  struct sockaddr_in broadcast_addr;
  SOCKET sock;
} platform_udpUnicaster_t;

struct platform_tcpClient_t_ {
  struct sockaddr_in addr;
  SOCKET sock;
};

typedef struct {
  platform_tcpClient_t client;
  pthread_t tid;
  bool running;
  platform_tcpServer_t *server;
} platform_tcpServer_clientInfo_t;

struct platform_tcpServer_t_ {
  struct sockaddr_in addr;
  SOCKET sock;
  platform_list_t *connected_clients;
  pthread_t tid;
};

#endif
