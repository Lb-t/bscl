#pragma once
#include <stdint.h>

typedef uint64_t os_microtime_t;
typedef uint32_t os_millitime_t;
typedef uint32_t os_time_t;


#define OS_MICROSECONDS_PER_TICK 1000
#define OS_TICKS_PER_SECOND (1000000 / OS_MICROSECONDS_PER_TICK)

#define os_msecs_to_ticks(msecs) ((msecs)*1000 / OS_MICROSECONDS_PER_TICK)
#define os_ticks_to_usecs(ticks) ((ticks)*1000000/OS_TICKS_PER_SECOND)


#if defined(__WIN32)
#include <windows.h>
//#define os_usleep(us)   usleep(us)
void bscl_os_usleep(long long usec);
#define bscl_os_msleep(ms) Sleep(ms)
#define bscl_os_sleep(s) bscl_os_msleep((s)*1000)
#define bscl_os_ticksleep(ticks) bscl_os_usleep(os_ticks_to_usecs(ticks))
#endif

os_microtime_t bscl_os_micro_uptime(void);
os_millitime_t bscl_os_milli_uptime(void);
#define bscl_os_uptime() (os_milli_uptime()/1000)

