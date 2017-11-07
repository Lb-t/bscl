#ifndef INCLUDE_PLATFORM_HTTP_
#define INCLUDE_PLATFORM_HTTP_
#include "io/platform_network_private.h"

typedef struct { platform_tcpClient_t tcpClient; } platform_http_client_t;

typedef enum {
  Platform_Http_Method_GET,
  Platform_Http_Method_HEAD,
  Platform_Http_Method_POST,
  Platform_Http_Method_PUT,
  Platform_Http_Method_DELETE,
  Platform_Http_Method_CONNECT,
  Platform_Http_Method_OPTIONS,
  Platform_Http_Method_TRACE
} Platform_Http_Method;

typedef enum {
  Platform_Http_Status_Continue = 100,
  Platform_Http_Status_SwitchingProtocols,
  Platform_Http_Status_OK = 200,
  Platform_Http_Status_Created,
  Platform_Http_Status_Accepted,
  Platform_Http_Status_NonAuthoritativeInformation,
  Platform_Http_Status_NoContent,
  Platform_Http_Status_ResetContent,
  Platform_Http_Status_PartialContent,
  Platform_Http_Status_MultipleChoices = 300,
  Platform_Http_Status_MovedPermanently,
  Platform_Http_Status_Found,
  Platform_Http_Status_SeeOther,
  Platform_Http_Status_NotModified,
  Platform_Http_Status_UseProxy,
  Platform_Http_Status_Unused,
  Platform_Http_Status_TemporaryRedirect,
  Platform_Http_Status_BadRequest = 400,
  Platform_Http_Status_Unauthorized,
  Platform_Http_Status_PaymentRequired,
  Platform_Http_Status_Forbidden,
  Platform_Http_Status_NotFound,
  Platform_Http_Status_MethodNotAllowed,
  Platform_Http_Status_NotAcceptable,
  Platform_Http_Status_ProxyAuthenticationRequired,
  Platform_Http_Status_RequestTimeout,
  Platform_Http_Status_Conflict,
  Platform_Http_Status_Gone,
  Platform_Http_Status_LengthRequired,
  Platform_Http_Status_PreconditionFailed,
  Platform_Http_Status_RequestEntityTooLarge,
  Platform_Http_Status_RequestURITooLarge,
  Platform_Http_Status_UnsupportedMediaType,
  Platform_Http_Status_Requestedrangenotsatisfiable,
  Platform_Http_Status_ExpectationFailed,
  Platform_Http_Status_InternalServerError = 500,
  Platform_Http_Status_NotImplemented,
  Platform_Http_Status_BadGateway,
  Platform_Http_Status_ServiceUnavailable,
  Platform_Http_Status_GatewayTimeout,
  Platform_Http_Status_HTTPVersionnotsupported
} Platform_Http_Status;

typedef enum {
  Platform_Http_Head_Allow,
  Platform_Http_Head_ContentEncoding,
  Platform_Http_Head_ContentLength,
  Platform_Http_Head_ContentType,
  Platform_Http_Head_Date,
  Platform_Http_Head_Expires,
  Platform_Http_Head_LastModified,
  Platform_Http_Head_Location,
  Platform_Http_Head_Refresh,
  Platform_Http_Head_Server,
  Platform_Http_Head_SetCookie,
  Platform_Http_Head_WWWAuthenticate
} Platform_Http_Head;
typedef struct{}platform_http_head_t;
typedef struct { Platform_Http_Method method,Platform_Http_Head head; } platform_http_request_t;
void platform_http_request_set_contents(platform_http_request_t *this,
                                        char *contents);
void platform_http_request_append_head(platform_http_request_t *this,  
                                       Platform_Http_Head head, char *value);
void platform_http_request(platform_http_client_t *this,Platform_Http_Method method,platform_http_head_t*heads);
#endif
