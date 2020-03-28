#include "bscl.h"
#include <Windows.h>
bscl_microtime_t bscl_micro_time(void)
{
    return bscl_milli_time() * 1000;
}
bscl_millitime_t bscl_milli_time(void)
{
    return GetTickCount();
}
void bscl_usleep(long long usec)
{
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -(10 * usec); // Convert to 100 nanosecond interval, negative
                                // value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}
