#include "bscl_os.h"
#include <Windows.h>
#if defined(_MSC_VER)
bscl_os_microtime_t bscl_os_micro_time(void)
{
    return bscl_os_milli_time() * 1000;
}
bscl_os_millitime_t bscl_os_milli_time(void)
{
    return GetTickCount();
}
void bscl_os_usleep(long long usec)
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
#endif
