#include "bscl_serialport.h"
#include <stdio.h>
#include "bscl_os.h"
#include <string.h>
#if 0
int b = 0;
#else
int a = 0;
#endif
void *recv_task(void *arg) {
  int fd;
  char c;
  memcpy(&fd, arg, sizeof(int));
  while (1) {
    int res = bscl_serialport_read(fd, &c, 1);
    if (res < 0) {
      break;
    }
    putchar(c);
  }
  return NULL;
}
int main(void) {
  char buf[1024];
  int fd = bscl_serialport_open("COM3");
  bscl_thread_t recv_tid;
  if (fd < 0) {
    return -1;
  }
  bscl_serialport_config_t conf = {
      .baudrate = 115200, .parity = BSCL_SERIALPORT_PARITY_NONE, .stopbits = BSCL_SERIALPORT_STOPBITS_1};
  bscl_serialport_config(fd, &conf);
  bscl_thread_create(&recv_tid, NULL, recv_task, &fd);
  while (1) {
    fgets(buf, 1024, stdin);
    int res = bscl_serialport_write(fd, buf, strlen(buf));
    if (res < 0) {
      break;
    }
  }
  return 0;
}
