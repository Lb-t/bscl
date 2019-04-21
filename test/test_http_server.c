#include "bscl_network.h"
#define BUF_SIZE 1024
char status[] = "HTTP/1.0 200 OK\r\n";
char header[] =
    "Server: A Simple Web Server\r\nContent-Type: text/html\r\n\r\n";
char body[] = "<html><head><title>A Simple Web "
              "Server</title></head><body><h2>Welcome!</h2><p>This is "
              "a simple test server!</p></body></html>";
int main(void) {
  int fd = bscl_tcp_new(8080);
  bscl_tcp_listen(fd);
  static char buf[BUF_SIZE];
  uint32_t ip;
  uint16_t port;
  while (1) {
    int client = bscl_tcp_accept(fd, &ip, &port);
    while (1) {
      int res = bscl_tcp_read(client, buf, BUF_SIZE);
      if (res <= 0) {
        break;
      }
       res=bscl_tcp_write(client, status, sizeof(status));
	  if (res < 0)
		  printf("res=%d\n", res);
      res=bscl_tcp_write(client, header, sizeof(header));
	  if (res < 0)
		  printf("res=%d\n", res);
     res= bscl_tcp_write(client, body, sizeof(body));
	 if (res < 0)
		 printf("res=%d\n", res);
    }
  }
}
