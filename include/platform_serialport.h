#pragma once 

enum platform_serialport_config_parity {
	PLATFORM_SERIALPORT_PARITY_NONE=0,
	PLATFORM_SERIALPORT_PARITY_EVEN,
	PLATFORM_SERIALPORT_PARITY_ODD,
};
enum platform_serialport_config_stopbits {
	PLATFORM_SERIALPORT_STOPBITS_1=0,
	PLATFORM_SERIALPORT_STOPBITS_1_5,
	PLATFORM_SERIALPORT_STOPBITS_2,
};
typedef struct  {
	int baudrate;
	int parity;
	int stopbits;
}platform_serialport_config_t;

typedef struct platform_serialport_t_ platform_serialport_t;

#define	PLATFORM_SERIALPORT_OK  0
#define PLATFORM_SERIALPORT_EARG -1
#define PLATFORM_SERIALPORT_EFAIL -2
#define PLATFORM_SERIALPORT_EMEM -3
#define PLATFORM_SERIALPORT_ESUPP -4



int platform_serialport_open(const char *name);
int platform_serialport_read(int fd, void *buf, int len);
int platform_serialport_close(int fd);
int platform_serialport_config(int fd, platform_serialport_config_t *conf);
int platform_serialport_write(int fd, void *buf, int len);
