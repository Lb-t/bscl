#pragma once
#include <stdint.h>

typedef uint64_t bscl_os_microtime_t;
typedef uint32_t bscl_os_millitime_t;

#define bscl_os_msecs_to_ticks(msecs) ((msecs)*1000 / BSCL_OS_MICROSECONDS_PER_TICK)

#if defined(_MSC_VER)
#include <windows.h>
void bscl_os_usleep(long long usec);
#define bscl_os_msleep(ms) Sleep(ms)
#define bscl_os_sleep(s) bscl_os_msleep((s)*1000)
$elif defined(__GNUC__)
#define bscl_os_sleep(t) _sleep(t*1000)
#define bscl_os_sleep_ms(t) _sleep(t)
#define os_usleep(us)   usleep(us)
#endif

bscl_os_microtime_t bscl_os_micro_time(void);
bscl_os_millitime_t bscl_os_milli_time(void);
