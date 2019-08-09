#include <stdio.h>
#include <stdint.h>
#include <string.h>
#if defined(_WIN32)
#include <windows.h>
#include <conio.h>
#elif defined(__linux__)
#include "curses.h"
#endif

#include "bscl_serialport.h"
#include "bscl_os.h"

#if defined(_WIN32)
#define DEV_NAME "COM5"
#elif defined(__linux__)
#define DEV_NAME "/dev/ttyS5"
#endif

void init(void) {
#if defined(_WIN32)
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
  DWORD mode = 0;
  GetConsoleMode(hStdin, &mode);
  SetConsoleMode(hStdin, mode & (~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT)));
#elif defined(__linux__)
  WINDOW *w;
  w = initscr();
  noecho();
  setbuf(stdout, NULL);
#endif
}

void deinit(void) {
#if defined(_WIN32)
#elif defined(__linux__)
  endwin();
#endif
}

void *recv_task(void *arg) {
  int fd = *(int *)arg;
  char c;
  while (1) {

    bscl_serialport_read(fd, &c, 1);
    putchar(c);
    // printf("\r\nrecv %x\n\r",c);

    fflush(stdin);
  }
  return NULL;
}

int main(void) {
  init();
  int fd = bscl_serialport_open(DEV_NAME);
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
    char buf = c;
    // printf("write %x\n\r",buf);
    bscl_serialport_write(fd, &buf, 1);
  }
  printf("exit\n");
  return 0;
}
