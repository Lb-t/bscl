#pragma once
#include <stdint.h>
#include "platform_network.h"
typedef struct platform_ftp_server_t_ platform_ftp_server_t;
#define platform_ftp_server_connected(fd,client_ip,client_port) platform_tcp_accept(fd,client_ip,client_port)
int platform_ftp_server_process(int client_fd);
platform_ftp_server_t *platform_ftp_server_new(uint32_t ip,uint16_t control_port);