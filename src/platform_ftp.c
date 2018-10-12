#if 0
#include "platform_ftp.h"
#include "platform_network.h"

#define FTP_CMD_ABOR "ABOR"
#define FTP_CMD_ACCT "ACCT"
#define FTP_CMD_ALLO
#define FTP_CMD_APPE
#define FTP_CMD_CCC
#define FTP_CMD_CWD
#define FTP_CMD_DELE
#define FTP_CMD_EPRT
#define FTP_CMD_EPSV
#define FTP_CMD_HELP
#define FTP_CMD_HOST
#define FTP_CMD_LIST
#define FTP_CMD_MDTM
#define FTP_CMD_MKD
#define FTP_CMD_PASV "PASV"
#define FTP_CMD_PORT

#define MAX_CMD_LEN 64

#define MAX_CLIENT_NUM 10

struct platform_ftp_server_t_ {
  int cmd_fd;
  int data_fd;
  int client_cmd_fd[MAX_CLIENT_NUM];
  int client_data_fd[MAX_CLIENT_NUM];
  uint32_t listen_ip;
  uint8_t client_num;
};

platform_ftp_server_t *platform_ftp_server_new(uint32_t ip, uint16_t control_port) {
  platform_ftp_server_t *server = (platform_ftp_server_t *)malloc(sizeof(platform_ftp_server_t));
  if (!server) {
    return NULL;
  }
  server->cmd_fd = platform_tcp_new(control_port);
  server->data_fd = platform_tcp_new(0);
  server->client_num = 0;
  server->listen_ip = ip;
  return server;
}
int platform_ftp_server_process(platform_ftp_server_t *server, int client_fd) {
  char cmd[MAX_CMD_LEN];
  cmd[MAX_CMD_LEN - 1] = 0;
  int i = 0;
  while (1 == MAX_CMD_LEN - 1) {
    if (i == MAX_CMD_LEN - 1) {
      i = 0;
    }
    platform_tcp_read(client_fd, cmd + i, 1);
    if (i > 0 && cmd[i] == '\n' && cmd[i - 1] == '\r') {
      /*check if it is a valid command*/
      cmd[i - 1] = 0;
      if (!strcmp(cmd, FTP_CMD_ABOR)) {
        sprintf(cmd, "227 entering passive mode (%d,%d,%d,%d,%d,%d)", *((uint8_t *)server->listen_ip),
                *((uint8_t *)server->listen_ip), *((uint8_t *)server->listen_ip), *((uint8_t *)server->listen_ip));
      } else if (!strcmp(cmd, FTP_CMD_ACCT)) {
      } else if (!strcmp(cmd, FTP_CMD_PASV)) {
        // pasive mode
      } else {
      }
      i = 0;
      continue;
    }
    ++i;
  }
}

#endif