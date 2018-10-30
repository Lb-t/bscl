#pragma once

#if defined(__linux)
#include "linux/platform_os_linux.h"
#elif defined(WIN32)
#include "win32/platform_os_win32.h"
#else
#error "not support os"
#endif

typedef platform_os_sem_t platform_sem_t;
typedef platform_os_mutex_t platform_mutex_t;