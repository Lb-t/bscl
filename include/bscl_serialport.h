#pragma once

enum bscl_serialport_config_parity {
  BSCL_SERIALPORT_PARITY_NONE = 0,
  BSCL_SERIALPORT_PARITY_EVEN,
  BSCL_SERIALPORT_PARITY_ODD,
};
enum bscl_serialport_config_stopbits {
  BSCL_SERIALPORT_STOPBITS_1 = 0,
  BSCL_SERIALPORT_STOPBITS_1_5,
  BSCL_SERIALPORT_STOPBITS_2,
};
typedef struct {
  int baudrate;
  int parity;
  int stopbits;
} bscl_serialport_config_t;

typedef struct bscl_serialport_t_ bscl_serialport_t;

#define BSCL_SERIALPORT_OK 0
#define BSCL_SERIALPORT_EARG -1
#define BSCL_SERIALPORT_EFAIL -2
#define BSCL_SERIALPORT_EMEM -3
#define BSCL_SERIALPORT_ESUPP -4

int bscl_serialport_open(const char *name);
int bscl_serialport_read(int fd, void *buf, int len);
int bscl_serialport_read_timeout(int fd, void *buf, int len, unsigned int timeout);
int bscl_serialport_close(int fd);
int bscl_serialport_config(int fd, bscl_serialport_config_t *conf);
int bscl_serialport_write(int fd, void *buf, int len);


int bscl_serialport_list_begin(void **sp_list);
int bscl_serialport_list_next(void *sp_list, char *name, unsigned long len);
int bscl_serialport_list_end(void *sp_list);
