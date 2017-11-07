#include "io/platform_network_private.h"

#define _TIMESPEC_DEFINED
#include <pthread.h>

#ifdef _WIN32
void platform_wsa_init(void) {
  WSADATA wsaData;
  static bool isInitialized = false;
  if (isInitialized)
    return;
  int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (res != 0) {
    perror("WSAStartup failed:\n");
    platform_abort();
  }
  isInitialized = true;
}
#endif

size_t platform_udpBroadcaster_write(platform_udpBroadcaster_t *this,
                                     void *data, size_t len) {
  platform_assert(this);
  platform_assert(data);
  platform_assert(len);
  int res = sendto(this->base.sock, data, len, 0,
                   (struct sockaddr *)&this->broadcast_addr,
                   sizeof(struct sockaddr_in));
  if (res < 0) {
    fprintf(stderr, "send failed.");
    perror("");
    return 0;
  }
  return res;
}

size_t platform_udpBroadcaster_read(platform_udpBroadcaster_t *this, void *data,
                                    size_t len) {
  platform_assert(this);
  platform_assert(data);
  platform_assert(len);
  struct sockaddr_in addr;
  int fromlen = sizeof(addr);
  int res;
  while (res = recvfrom(this->base.sock, data, len, 0, (struct sockaddr *)&addr,
                        &fromlen)) {
    if (res > 0) {
      if (addr.sin_addr.s_addr != this->local_ip)
        return res;
    } else {
      return 0;
    }
    // printf("recvfrom failed res=%d err=%d\n",res, WSAGetLastError());
  }
  // should never go here
  platform_abort();
  return 0;
}

bool platform_udpBase_init(platform_udpBase_t *this, uint16_t port) {
  platform_assert(this);
  platform_assert(port > 1024U);
#ifdef _WIN32
  platform_wsa_init();
#endif
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = 0;
  this->sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (this->sock < 0) {
    perror("socket failed.");
    return false;
  }

  if (bind(this->sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind error.");
    return false;
  }
  return true;
}

bool platform_udpBroadcaster_init(platform_udpBroadcaster_t *this,
                                  uint16_t local_port, uint16_t remote_port) {
  platform_assert(this);
  if (!platform_udpBase_init((platform_udpBase_t *)this, local_port))
    return false;

  this->broadcast_addr.sin_family = AF_INET;
  this->broadcast_addr.sin_port = htons(local_port);
  this->broadcast_addr.sin_addr.s_addr = INADDR_BROADCAST;
// enable broadcast
#ifdef _WIN32
  char
#else
  int
#endif
      bOpt = true;
  int res = setsockopt(this->base.sock, SOL_SOCKET, SO_BROADCAST, &bOpt,
                       sizeof(bOpt));
  if (res < 0) {
    if (this->base.sock < 0) {
      perror("socket failed.");
      return false;
    }
  }
  // get local ip
  struct sockaddr_in temp_addr;
  char get_local_ip[] = "get local ip";
  res = sendto(this->base.sock, get_local_ip, sizeof(get_local_ip), 0,
               (struct sockaddr *)&this->broadcast_addr,
               sizeof(struct sockaddr_in));
  if (res < 0) {
    perror("send to failed.");
    return false;
  }
  int addr_len = sizeof(temp_addr);
  res = recvfrom(this->base.sock, get_local_ip, sizeof(get_local_ip), 0,
                 (struct sockaddr *)&temp_addr, &addr_len);

  this->local_ip = temp_addr.sin_addr.s_addr;
  //	printf("my ip: %s\n", inet_ntoa(temp_addr.sin_addr));
  this->broadcast_addr.sin_port = htons(remote_port);
  return true;
}

void platform_udpBroadcaster_deinit(platform_udpBroadcaster_t *this) {
  platform_assert(this);
#ifdef _WIN32
  shutdown(this->base.sock, SD_BOTH);
  closesocket(this->base.sock);
#else
  shutdown(this->base.sock, SHUT_RDWR);
  close(this->base.sock);
#endif
}

platform_udpBroadcaster_t *platform_udpBroadcaster_new(uint16_t local_port,
                                                       uint16_t remote_port) {
  platform_udpBroadcaster_t *caster =
      (platform_udpBroadcaster_t *)malloc(sizeof(platform_udpBroadcaster_t));
  if (!caster)
    return NULL;
  if (!platform_udpBroadcaster_init(caster, local_port, remote_port)) {
    free(caster);
    caster = NULL;
  }
  return caster;
}

void platform_udpBroadcaster_delete(platform_udpBroadcaster_t *this) {
  platform_assert(this);
  platform_udpBroadcaster_deinit(this);
  free(this);
}
bool platform_udpMulticaster_init(platform_udpMulticaster_t *this,
                                  uint16_t local_port, uint16_t remote_port) {
  platform_assert(this);
  if (!platform_udpBase_init((platform_udpBase_t *)this, local_port))
    return false;
  unsigned char ttl = 255;
  setsockopt(this->base.sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
  struct in_addr addr;
  setsockopt(this->base.sock, IPPROTO_IP, IP_MULTICAST_IF, &addr, sizeof(addr));
  unsigned char loop = 0;
  setsockopt(this->base.sock, IPPROTO_IP, IP_MULTICAST_LOOP, &loop,
             sizeof(loop));
  struct ip_mreq ipmr;
  ipmr.imr_interface.s_addr = htonl(INADDR_ANY);
  ipmr.imr_multiaddr.s_addr = inet_addr("234.5.6.7");
  setsockopt(this->base.sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&ipmr,
             sizeof(ipmr));
  return true;
}
bool platform_tcpClient_connect(platform_tcpClient_t *this,
                                unsigned long server_ip, uint16_t server_port) {
  platform_assert(server_port > 1024U);
  platform_assert(server_ip);
  struct sockaddr_in addr;
  // connect to server
  addr.sin_family = AF_INET;
  addr.sin_port = htons(server_port);
  addr.sin_addr.s_addr = server_ip;
  if (connect(this->sock, (struct sockaddr *)&addr,
              sizeof(struct sockaddr_in)) < 0) {
    return false;
  }
  return true;
}
bool platform_tcpClient_init(platform_tcpClient_t *this, uint16_t port) {
  platform_assert(this);
  platform_assert(port > 1024U);

#ifdef _WIN32
  platform_wsa_init();
#endif
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = 0;
  this->sock = socket(AF_INET, SOCK_STREAM, 0);
  if (this->sock < 0) {
    fprintf(stderr, "socket failed.\n");
    return false;
  }
  int res;
  if (res = bind(this->sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    fprintf(stderr, "bind error.\n");
    return false;
  }
  return true;
}

void platform_tcpClient_deinit(platform_tcpClient_t *this) {

#ifdef _WIN32
  shutdown(this->sock, SD_BOTH);
  closesocket(this->sock);
#else
  shutdown(this->sock, SHUT_RDWR);
  close(this->sock);
#endif
}
platform_tcpClient_t *platform_tcpClient_new(uint16_t port) {
  platform_tcpClient_t *client =
      (platform_tcpClient_t *)malloc(sizeof(platform_tcpClient_t));
  if (!client) {
    return NULL;
  }
  if (!platform_tcpClient_init(client, port)) {
    free(client);
    return NULL;
  }
  return client;
}

void platform_tcpClient_delete(platform_tcpClient_t *this) {
  platform_tcpClient_deinit(this);
  free(this);
}

size_t platform_tcpClient_read(platform_tcpClient_t *this, void *buf,
                               size_t len) {
  platform_assert(this);
  platform_assert(buf);
  platform_assert(len);

  int res = recv(this->sock, buf, len, 0);
  if (res <= 0) {
    perror("read failed.");
    return 0;
  }
  return res;
}

size_t platform_tcpClient_write(platform_tcpClient_t *this, void *buf,
                                size_t len) {
  platform_assert(this);
  platform_assert(buf);
  platform_assert(len);
  int res = send(this->sock, buf, len, 0);
  if (res < 0) {
    perror("send failed.");
    return 0;
  }
  return res;
}
/*tcp server receive call back*/
platform_tcpServerCallback_t platform_tcpServer_recvCallback = NULL;

void platform_tcpServer_removeClient(platform_tcpServer_clientInfo_t *info) {
  platform_assert(info);
  platform_tcpServer_t *this = info->server;
  platform_list_item_t *item = platform_list_head(this->connected_clients);
  platform_tcpServer_clientInfo_t *temp;
  while (item) {
    platform_list_item_data(item, &temp);
    if (info == temp) {
      // platform_tcpClient_deinit(&info->client);
      free(info);
      platform_list_remove(this->connected_clients, item);
      return;
    }
    item = platform_list_item_next(item);
  }
}

void *platform_tcpServer_clientThread(void *arg) {
  platform_tcpServer_clientInfo_t *info =
      (platform_tcpServer_clientInfo_t *)arg;
  platform_tcpClient_t *client = (platform_tcpClient_t *)arg;
  printf("new connection,ip: %s port: %d\n", inet_ntoa(client->addr.sin_addr),
         ntohs(client->addr.sin_port));
  char *buf = malloc(1024);
  while (info->running) {
    int res = recv(client->sock, buf, 1024, 0);
    if (res < 0) {
      fprintf(stderr, "recv error .\n");
      abort();
    } else if (res == 0) {
      // shutdown
      printf("tcp client shutdown.\n");
      platform_tcpServer_removeClient(info);
      return (void *)0;
    }
    platform_assert(platform_tcpServer_recvCallback);
    platform_tcpServer_recvCallback(client, buf, res);
  }
  return (void *)1;
}

void *platform_tcpServer_thread(void *arg) {
  platform_tcpServer_t *this = (platform_tcpServer_t *)arg;
  assert(this);
  assert(this->connected_clients);
  assert(this->sock);

  int addrlen = sizeof(struct sockaddr_in);
  int res;
  platform_tcpServer_clientInfo_t *info;
  while (1) {
    info = (platform_tcpServer_clientInfo_t *)malloc(
        sizeof(platform_tcpServer_clientInfo_t));
    if (!info) {
      fprintf(stderr, "malloc failed.\n");
    }
    res = accept(this->sock, (struct sockaddr *)&info->client.addr, &addrlen);
    if (res < 0) {
      fprintf(stderr, "accept error.\n");
      abort();
    }
    info->client.sock = res;
    info->running = true;
    info->server = this;
    if (!platform_list_insert(this->connected_clients, NULL, info,
                              sizeof(platform_tcpClient_t))) {
      fprintf(stderr, "list insert error.\n");
      abort();
    }
    pthread_t tid;
    res = pthread_create(&tid, NULL, platform_tcpServer_clientThread, info);

    if (res < 0) {
      fprintf(stderr, "pthread create error.\n");
      abort();
    }
    info->tid = tid;
  }
}

bool platform_tcpServer_init(platform_tcpServer_t *this, uint16_t port,
                             platform_tcpServerCallback_t callback) {
  platform_assert(this);
  platform_assert(port > 1024);
  platform_assert(callback);

  platform_tcpServer_recvCallback = callback;
  this->connected_clients = platform_list_new();
  if (!this->connected_clients) {

    return false;
  }
#ifdef _WIN32
  platform_wsa_init();
#endif
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = 0;
  this->sock = socket(AF_INET, SOCK_STREAM, 0);
  if (this->sock < 0) {
    fprintf(stderr, "socket failed.\n");
    return false;
  }
  int res;
  if (res = bind(this->sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    fprintf(stderr, "bind error.\n");
    return false;
  }

  res = listen(this->sock, SOMAXCONN);
  if (res < 0) {
    fprintf(stderr, "listen error.\n");
    abort();
  }

  // create a new thread to accept connection
  pthread_t tid;
  res = pthread_create(&tid, NULL, platform_tcpServer_thread, this);
  if (res < 0) {
    fprintf(stderr, "pthread create error.\n");
    abort();
  }
  return true;
}

void platform_tcpServer_deinit(platform_tcpServer_t *this) {
  platform_assert(this);

  // deninit all client
  platform_list_item_t *item = platform_list_head(this->connected_clients);
  platform_tcpServer_clientInfo_t *info;

  /*remove all item,deinit the client,stop all client's thread*/
  while (item) {
    size_t len = platform_list_item_data(item, &info);
    platform_assert(len == sizeof(platform_tcpServer_clientInfo_t));
    info->running = false;
    platform_tcpClient_deinit(&info->client);

    // wait the thread exit
    void *return_value;
    pthread_join(info->tid, &return_value);
    free(info);
    platform_list_item_t *temp = item;
    item = platform_list_item_next(item);
    platform_list_remove(this->connected_clients, temp);
  }
  /*delete lists*/
  platform_list_delete(this->connected_clients);

  /*stop accept thread*/
}

platform_tcpServer_t *
platform_tcpServer_new(uint16_t port, platform_tcpServerCallback_t callback) {
  platform_tcpServer_t *server =
      (platform_tcpServer_t *)malloc(sizeof(platform_tcpServer_t));
  if (!server)
    return NULL;
  if (!platform_tcpServer_init(server, port, callback)) {
    free(server);
    return NULL;
  }
  return server;
}

void platform_tcpServer_delete(platform_tcpServer_t *this) {
  platform_tcpServer_deinit(this);
  free(this);
}
