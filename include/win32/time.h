#pragma once
#include <stdint.h>

typedef uint64_t bscl_microtime_t;
typedef uint32_t bscl_millitime_t;
typedef uint32_t bscl_time_t;


#define OS_MICROSECONDS_PER_TICK 1000
#define OS_TICKS_PER_SECOND (1000000 / OS_MICROSECONDS_PER_TICK)

#define os_msecs_to_ticks(msecs) ((msecs)*1000 / OS_MICROSECONDS_PER_TICK)
#define os_ticks_to_usecs(ticks) ((ticks)*1000000/OS_TICKS_PER_SECOND)


#if defined(_WIN32)
void bscl_usleep(long long usec);
#define bscl_msleep(ms) bscl_usleep((ms)*1000)
#define bscl_sleep(s) bscl_msleep((s)*1000)
#define bscl_ticksleep(ticks) bscl_usleep(os_ticks_to_usecs(ticks))
#endif

bscl_microtime_t bscl_micro_time(void);
bscl_millitime_t bscl_milli_time(void);
#define bscl_time() (os_milli_uptime()/1000)

