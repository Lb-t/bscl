#pragma once

#if defined(__linux)
#include "./linux/mutex.h"
#include "./linux/process.h"
#include "./linux/sem.h"
#include "./linux/thread.h"
#include "./linux/time.h"
#include "./linux/timer.h"
#elif defined(WIN32)
#include "./win32/mutex.h"
#include "./win32/process.h"
#include "./win32/sem.h"
#include "./win32/thread.h"
#include "./win32/time.h"
#include "./win32/timer.h"
#elif defined(__APPLE__)
#include "darwin/bscl_os_darwin.h"
#else
#error "not support os"
#endif

typedef bscl_os_sem_t bscl_sem_t;
typedef bscl_os_mutex_t bscl_mutex_t;
typedef bscl_os_thread_t bscl_thread_t;
typedef bscl_os_process_t bscl_process_t;
