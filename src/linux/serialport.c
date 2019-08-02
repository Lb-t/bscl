#include <poll.h>
#include <termios.h>
#include <fcntl.h>
#include "bscl_serialport.h"

int bscl_serialport_open(const char *name) {
  int fd = open(name, O_RDWR | O_NOCTTY);
  if (fd < 0) {
    return -1;
  }
}
int bscl_serialport_read(int fd, void *buf, int len) {
  return read(fd, buf, len);
}
int bscl_serialport_read_timeout(int fd, void *buf, int len, unsigned int timeout) {
  struct pollfd fds[1];
  int ret;
  memset(fds, 0, sizeof(fds));
  fds[0].fd = fd;
  fds[0].events = POLLIN;
  ret = poll(fds, 1, timeout);

  if (ret > 0 && (fds[0].revents & POLLIN)) {
    ret = read(fd, buf, len);
    if (ret <= 0)
      return -1;
    return ret;
  }
  if (ret == 0)
    return 0;

  return -1;
}
int bscl_serialport_close(int fd) {
  close(fd);
  return 0;
}
int bscl_serialport_config(int fd, bscl_serialport_config_t *conf) {
  struct termios opt;

  tcgetattr(fd, &opt);
  cfsetispeed(&opt, conf->baudrate);
  cfsetospeed(&opt, conf->baudrate);

  opt.c_cflag &= ~CSIZE;
  opt.c_cflag |= CS8;
  opt.c_cflag &= ~PARENB;
  opt.c_cflag &= ~INPCK;
  opt.c_cflag |= (CLOCAL | CREAD);

  opt.c_cflag &= ~CRTSCTS;
  if (conf->stopbits == BSCL_SERIALPORT_STOPBITS_2) {
    opt.c_cflag |= CSTOPB;
  } else {
    opt.c_cflag &= ~CSTOPB;
  }

  opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  opt.c_oflag &= ~OPOST;
  opt.c_oflag &= ~(ONLCR | OCRNL);

  opt.c_iflag &= ~(ICRNL | INLCR);
  opt.c_iflag &= ~(IXON | IXOFF | IXANY);

  opt.c_cc[VTIME] = 0;
  opt.c_cc[VMIN] = 0;

  cfmakeraw(&opt);

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd, TCSANOW, &opt) != 0) {
    close(fd);
    return -1;
  }
  return 0;
}

int bscl_serialport_write(int fd, void *buf, int len) {
  return write(fd, buf, len);
}

int bscl_serialport_list_begin(void **sp_list) {
  return 0;
}
int bscl_serialport_list_next(void *sp_list, char *name, unsigned long len);
int bscl_serialport_list_end(void *sp_list);
