#include "platform_http.h"

const char *platform_http_method_str[] = {"GET", "HEAD"};
const char platform_http_version_str[] = "HTTP/1.1\r\n";
void platform_http_client_request(platform_http_client_t *client,
                                  const char *url,
                                  const Platform_Http_Method method,
                                  const char *heads, char *body) {
  int res;
  res = platform_tcp_write(client->fd, platform_http_method_str[method],
                           strlen(platform_http_method_str[method]));

  res = platform_tcp_write(client->fd, " ", 1);

  res = platform_tcp_write(client->fd, url, strlen(url));
  res = platform_tcp_write(client->fd, " ", 1);
  res = platform_tcp_write(client->fd, platform_http_version_str,
                           strlen(platform_http_version_str));

  res = platform_tcp_write(client->fd, heads, strlen(heads));

  printf("exit request\n");
}