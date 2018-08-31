
#include "platform_network.h"
#define _TIMESPEC_DEFINED
#include <pthread.h>
void *test_udp(void *arg) {
  puts("test udp");
  int udpClient = platform_udp_new(1256);
  static char buf[1024];
  uint32_t ip;
  uint16_t port;
  int length;
  while (1) {
    length = platform_udp_receive(udpClient, buf, 1024, &ip, &port);

    platform_udp_unicast(udpClient, buf, length, ip, port);
    length = sprintf(buf, "receive length=%d from ip=%d,port=%d\n", length, ip,
                     port);
    platform_udp_broadcast(udpClient, buf, length, port);
  }
  return NULL;
}

void *test_tcp_server_conn(void *arg) {
  int fd = *(int *)arg;
  printf("fd=%d\n", fd);
  free(arg);
  char *buf = malloc(1024);
  while (1) {
    int res = platform_tcp_read(fd, buf, 1024);
    if (res > 0) {
      platform_tcp_write(fd, buf, res);
    } else if (res == 0) {
      puts("disconnect tcp");
      break;
    } else {
      fprintf(stderr, "tcp read failed %d %d", res, WSAGetLastError());
      break;
    }
  }
  return NULL;
}

void *test_tcp_server(void *arg) {
  puts("test tcp");
  int tcpServer = platform_tcp_new(1588);
  platform_tcp_listen(tcpServer);
  uint32_t ip;
  uint16_t port;
  int fd;
  while (1) {
    fd = platform_tcp_accept(tcpServer, &ip, &port);
    printf("new connection ip=%d.%d.%d.%d,port=%d fd=%d\n", ((uint8_t *)&ip)[0],
           ((uint8_t *)&ip)[1], ((uint8_t *)&ip)[2], ((uint8_t *)&ip)[3],
           ntohs(port), fd);
    pthread_t tid;
    int *arg = malloc(sizeof(int));
    *arg = fd;
    pthread_create(&tid, NULL, test_tcp_server_conn, arg);
  }
}

void *test_tcp_client(void *arg) {
  int tcpClient = platform_tcp_new(0);
  static char buf[1024];
  int length;
  platform_tcp_connect(tcpClient, inet_addr("172.28.1.4"), 1588);
  length = sprintf(buf, "hello,this is a tcp test.");
  platform_tcp_write(tcpClient, buf, length);
  while (1) {
    length = platform_tcp_read(tcpClient, buf, 1024);
    printf("tcp client receive length=%d\n", length);
    if (length > 0) {
      platform_tcp_write(tcpClient, buf, length);
    } else if (length == 0) {
      printf("tcp client disconnect\n");
      break;
    } else {
      printf("receive error\n");
      break;
    }
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  pthread_t tid;
  puts("test");
  pthread_create(&tid, NULL, test_udp, NULL);
  pthread_create(&tid, NULL, test_tcp_server, NULL);
  pthread_create(&tid, NULL, test_tcp_client, NULL);
  void *retval;
  pthread_join(tid, &retval);
}