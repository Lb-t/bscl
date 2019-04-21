#pragma once
#include "bscl_network.h"
#include <stdint.h>
#define BSCL_HTTP_BUF_SIZE 1024
typedef struct {
  int fd;
  char buf[BSCL_HTTP_BUF_SIZE + 1];
} bscl_http_client_t;

typedef enum {
  Bscl_Http_Method_GET = 0,
  Bscl_Http_Method_HEAD,
  Bscl_Http_Method_POST,
  Bscl_Http_Method_PUT,
  Bscl_Http_Method_DELETE,
  Bscl_Http_Method_CONNECT,
  Bscl_Http_Method_OPTIONS,
  Bscl_Http_Method_TRACE
} Bscl_Http_Method;

typedef enum {
  Bscl_Http_Status_Continue = 100,
  Bscl_Http_Status_SwitchingProtocols,
  Bscl_Http_Status_OK = 200,
  Bscl_Http_Status_Created,
  Bscl_Http_Status_Accepted,
  Bscl_Http_Status_NonAuthoritativeInformation,
  Bscl_Http_Status_NoContent,
  Bscl_Http_Status_ResetContent,
  Bscl_Http_Status_PartialContent,
  Bscl_Http_Status_MultipleChoices = 300,
  Bscl_Http_Status_MovedPermanently,
  Bscl_Http_Status_Found,
  Bscl_Http_Status_SeeOther,
  Bscl_Http_Status_NotModified,
  Bscl_Http_Status_UseProxy,
  Bscl_Http_Status_Unused,
  Bscl_Http_Status_TemporaryRedirect,
  Bscl_Http_Status_BadRequest = 400,
  Bscl_Http_Status_Unauthorized,
  Bscl_Http_Status_PaymentRequired,
  Bscl_Http_Status_Forbidden,
  Bscl_Http_Status_NotFound,
  Bscl_Http_Status_MethodNotAllowed,
  Bscl_Http_Status_NotAcceptable,
  Bscl_Http_Status_ProxyAuthenticationRequired,
  Bscl_Http_Status_RequestTimeout,
  Bscl_Http_Status_Conflict,
  Bscl_Http_Status_Gone,
  Bscl_Http_Status_LengthRequired,
  Bscl_Http_Status_PreconditionFailed,
  Bscl_Http_Status_RequestEntityTooLarge,
  Bscl_Http_Status_RequestURITooLarge,
  Bscl_Http_Status_UnsupportedMediaType,
  Bscl_Http_Status_Requestedrangenotsatisfiable,
  Bscl_Http_Status_ExpectationFailed,
  Bscl_Http_Status_InternalServerError = 500,
  Bscl_Http_Status_NotImplemented,
  Bscl_Http_Status_BadGateway,
  Bscl_Http_Status_ServiceUnavailable,
  Bscl_Http_Status_GatewayTimeout,
  Bscl_Http_Status_HTTPVersionnotsupported
} Bscl_Http_Status;

typedef enum {
  Bscl_Http_Content_OctetStream,
  Bscl_Http_Content_TextHtml,
  Bscl_Http_Content_TextXml,
  Bscl_Http_Content_TextPlain,
  Bscl_Http_Content_Others
} Bscl_Http_Content_Type;

typedef enum {
  Bscl_Http_Head_Allow,
  Bscl_Http_Head_ContentEncoding,
  Bscl_Http_Head_ContentLength,
  Bscl_Http_Head_ContentType,
  Bscl_Http_Head_Date,
  Bscl_Http_Head_Expires,
  Bscl_Http_Head_LastModified,
  Bscl_Http_Head_Location,
  Bscl_Http_Head_Refresh,
  Bscl_Http_Head_Server,
  Bscl_Http_Head_SetCookie,
  Bscl_Http_Head_WWWAuthenticate
} Bscl_Http_Head;
typedef struct {
	uint8_t a;
} bscl_http_head_t;
typedef struct {
  Bscl_Http_Method method;
  Bscl_Http_Head head;
} bscl_http_request_t;

typedef struct {
  Bscl_Http_Status status;
  Bscl_Http_Content_Type content_type;
  int content_length;
  bool completed_head;
  char *contents;
} bscl_http_response_t;

void bscl_http_request_set_contents(bscl_http_request_t *request,
                                        char *contents);
void bscl_http_request_append_head(bscl_http_request_t *request,
                                       Bscl_Http_Head head, char *value);
void bscl_http_client_request(bscl_http_client_t *client,
                                  const char *url,
                                  const Bscl_Http_Method method,
                                  const char *heads, char *body,
                                  bscl_http_response_t *const response);

#define bscl_http_client_connect(client, ip)                               \
  bscl_http_client_connect_with_port(client, ip, 80)
#define bscl_http_client_connect_with_port(client, ip, port)               \
  bscl_tcp_connect((client)->fd, ip, port)

