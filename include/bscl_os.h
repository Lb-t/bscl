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
#include "./win32/fs.h"
#elif defined(__APPLE__)
#include "./apple/mutex.h"
#include "./apple/process.h"
#include "./apple/sem.h"
#include "./apple/thread.h"
#include "./apple/time.h"
#include "./apple/timer.h"
#else
#error "not support os"
#endif

