#pragma once
#include <stdint.h>
#include <unistd.h>
typedef uint64_t bscl_os_microtime_t;
typedef uint32_t bscl_os_millitime_t;

#define bscl_os_usleep(us) usleep(us)
#define bscl_os_msleep(ms) usleep((ms)*1000)
#define bscl_os_sleep(s) bscl_os_msleep((s)*1000)

bscl_os_microtime_t bscl_os_micro_time(void);
bscl_os_millitime_t bscl_os_milli_time(void);
