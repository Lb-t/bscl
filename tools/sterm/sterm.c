#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>
#include "bscl_serialport.h"
#include "bscl_os.h"
#include <conio.h>

void init(void) {
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
  DWORD mode = 0;
  GetConsoleMode(hStdin, &mode);
  SetConsoleMode(hStdin, mode & (~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT)));
}

void *recv_task(void *arg) {
  int fd = *(int *)arg;
  char c;
  while (1) {
    bscl_serialport_read(fd, &c, 1);
    putchar(c);
  }
  return NULL;
}

int main(void) {
  init();
  int fd = bscl_serialport_open("COM3");
  if (fd < 0) {
    printf("fail\n");
    getchar();
    return -1;
  } else {
    // configue
    bscl_serialport_config_t conf = {
        .baudrate = 115200, .parity = BSCL_SERIALPORT_PARITY_NONE, .stopbits = BSCL_SERIALPORT_STOPBITS_1};
    if (bscl_serialport_config(fd, &conf) < 0) {
      printf("configure fail\n");
      getchar();
      return -1;
    }
  }

  // creat thread
  bscl_os_thread_t tid;
  bscl_os_thread_create(&tid, NULL, recv_task, &fd);
  while (1) {
    int c = getch();
    if (c < 0) {
      continue;
    } else {
      char buf = c;
      bscl_serialport_write(fd, &buf, 1);
    }
  }
  printf("exit\n");
  return 0;
}
