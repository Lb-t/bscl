#pragma once
#include <stdint.h>
#include <unistd.h>
typedef uint64_t bscl_microtime_t;
typedef uint32_t bscl_millitime_t;

#define bscl_usleep(us) usleep(us)
#define bscl_msleep(ms) usleep((ms)*1000)
#define bscl_sleep(s) bscl_msleep((s)*1000)

bscl_microtime_t bscl_micro_time(void);
bscl_millitime_t bscl_milli_time(void);
