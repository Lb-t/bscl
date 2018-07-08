#include "platform_http.h"
#include <stdio.h>
int main(int argc, char *argv[]) {
  platform_http_client_t client;
  static char buf[1024];
  client.fd = platform_tcp_new(2345);
  client.buf[PLATFORM_HTTP_BUF_SIZE] = 0;
  int res = platform_http_client_connect(&client, inet_addr("14.17.32.211"));
  if (res) {
    printf("connect failed.res=%d\n", res);
    return 0;
  }

  platform_http_response_t response;
  platform_http_client_request(&client, "/", Platform_Http_Method_GET,
                               "Host: www.qq.com\r\nUser-Agent: Mozilla/5.0 "
                               "(Windows NT 10.0; Win64; x64) "
                               "AppleWebKit/537.36 (KHTML, like Gecko) "
                               "Chrome/63.0.3239.84 Safari/537.36\r\n\r\n",
                               "", &response);

  return 0;
}
