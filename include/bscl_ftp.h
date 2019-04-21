#pragma once
#include "bscl_network.h"
#include <stdint.h>
typedef struct bscl_ftp_server_t_ bscl_ftp_server_t;
#define bscl_ftp_server_connected(fd, client_ip, client_port) bscl_tcp_accept(fd, client_ip, client_port)
int bscl_ftp_server_process(int client_fd);
bscl_ftp_server_t *bscl_ftp_server_new(uint32_t ip, uint16_t control_port);
