#pragma once

#if defined(__linux)
#include "linux/bscl_os_linux.h"
#elif defined(WIN32)
#if defined(_MSC_VER)
#include "win32/bscl_os_win32.h"
#else 
#include "win32/bscl_os_mingw.h"
#endif
#elif defined(__APPLE__)
#include "darwin/bscl_os_darwin.h"
#else
#error "not support os"
#endif

typedef bscl_os_sem_t bscl_sem_t;
typedef bscl_os_mutex_t bscl_mutex_t;
