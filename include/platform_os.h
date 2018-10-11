#pragma once

#if defined(__linux)
#include "linux/platform_os_linux.h"
#elif defined(WIN32)
#include "win32/platform_os_win32.h"
#else
#error "not support os"
#endif
