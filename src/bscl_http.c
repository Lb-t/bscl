#include "bscl_http.h"
#include <stdlib.h>
const char *bscl_http_method_str[] = {"GET", "HEAD"};
const char bscl_http_version_str[] = "HTTP/1.1\r\n";

// return parsed len
int bscl_http_parser_header(char *buf, size_t len, bscl_http_response_t *const response) {
  size_t remain_len = len;
  char *current_line = NULL;
  char *next_line = buf;
  response->content_length = -1;
  char *temp = strstr(buf, "HTTP/");
  if (temp != buf) {
    fprintf(stderr, "not a http message");
    return -1;
  }
  while (1) {
    current_line = next_line;
    // seek to next line
    unsigned int i = 0;
    while ((i + 1) < remain_len && !(current_line[i] == '\r' && current_line[i + 1] == '\n')) {
      ++i;
    }
    if (i == 0) {
      printf("found head end\n");
      response->completed_head = true;
      remain_len -= 2;
      break;
    }
    if (i + 1 < remain_len) {
      next_line = current_line + i + 2;
      remain_len = remain_len - i - 2;
    } else {
      // not a full line
      fprintf(stderr, "not a full line i=%d len=%zu remain=%zu\n", i, len, remain_len);
      return len - remain_len;
    }

    if (strstr(current_line, "Content-Length:") == current_line) {
      if (sscanf(current_line + sizeof("Content-Length:") - 1, "%d", &response->content_length) > 0) {
        printf("Content-Length=%d\n", response->content_length);
      }
      continue;
    }

    if (strstr(current_line, "Content-Type:") == current_line) {
      temp = current_line + sizeof("Content-Type:") - 1;
      char type[20] = {0};
      unsigned int i = 0, j = 0;
      size_t len = next_line - current_line;
      do {
        if (temp[i] == ' ')
          continue;
        else if (temp[i] == '\r' || temp[i] == '\n')
          break;
        else {
          type[j++] = temp[i];
        }
      } while (++i < len);
      type[j] = 0;
      if (0 == strcmp(type, "text/plain")) {
        response->content_type = Bscl_Http_Content_TextPlain;
      } else if (0 == strcmp(type, "text/xml")) {
        response->content_type = Bscl_Http_Content_TextXml;
      } else if (0 == strcmp(type, "text/html")) {
        response->content_type = Bscl_Http_Content_TextHtml;
      } else if (0 == strcmp(type, "octet-stream")) {
        response->content_type = Bscl_Http_Content_OctetStream;
      } else {
        response->content_type = Bscl_Http_Content_Others;
      }
      printf("Content-Type:%s\n", type);
    }
  }
  return len - remain_len;
}

void bscl_http_client_request(bscl_http_client_t *client, const char *url, const Bscl_Http_Method method, const char *heads,
                              char *body, bscl_http_response_t *const response) {
  int res;
  res = bscl_tcp_write(client->fd, bscl_http_method_str[method], strlen(bscl_http_method_str[method]));

  res = bscl_tcp_write(client->fd, " ", 1);

  res = bscl_tcp_write(client->fd, url, strlen(url));
  res = bscl_tcp_write(client->fd, " ", 1);
  res = bscl_tcp_write(client->fd, bscl_http_version_str, strlen(bscl_http_version_str));

  res = bscl_tcp_write(client->fd, heads, strlen(heads));
  FILE *fp = fopen("test_http.txt", "w+");

  char *recv_buf = client->buf;
  char *parse_buf = client->buf;
  do {
    if (recv_buf == client->buf + BSCL_HTTP_BUF_SIZE) {
      //
      printf("http head is too long\n");
      return;
    }

    res = bscl_tcp_read(client->fd, recv_buf, BSCL_HTTP_BUF_SIZE - (recv_buf - client->buf));

    if (res <= 0) {
      return;
    } else {
      fwrite(recv_buf, 1, res, fp);
      recv_buf += res;
      res = bscl_http_parser_header(parse_buf, recv_buf - parse_buf, response);
      parse_buf += res;
    }
  } while (!response->completed_head);

  if (response->content_length > 0) {
    response->contents = malloc(response->content_length);
    size_t len = recv_buf - parse_buf;
    memcpy(response->contents, parse_buf, len);
    while (len < response->content_length) {
      res = bscl_tcp_read(client->fd, client->buf, BSCL_HTTP_BUF_SIZE);
      if (res > 0) {
        fwrite(client->buf, 1, res, fp);
        memcpy(response->contents + len, client->buf, res);
        len += res;
      } else {
        break;
      }
    }
  }
  fclose(fp);
}
