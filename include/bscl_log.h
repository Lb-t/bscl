#pragma once
#include "bscl_common.h"
#include <time.h>
#define bscl_logger_t FILE
#define bscl_logger_new(name) (fopen(name, "w+"))
#define bscl_logger_delete(name) (fclose(name))
#ifdef NDEBUG
#define BSCL_LOG(logger, format, ...)                                                                                          \
  do {                                                                                                                         \
    bscl_assert(logger);                                                                                                       \
    time_t timep;                                                                                                              \
    struct tm *p;                                                                                                              \
    time(&timep);                                                                                                              \
    p = localtime(&timep);                                                                                                     \
    fprintf(logger, "[%04d%02d%02d %02d:%02d:%02d]" format "\n", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour,     \
            p->tm_min, p->tm_sec, ##__VA_ARGS__);                                                                              \
    fflush(logger);                                                                                                            \
  } while (0)
#else
#define BSCL_LOG(logger, format, ...)                                                                                          \
  do {                                                                                                                         \
    bscl_assert(logger);                                                                                                       \
    time_t timep;                                                                                                              \
    struct tm *p;                                                                                                              \
    time(&timep);                                                                                                              \
    p = localtime(&timep);                                                                                                     \
    fprintf(logger, "[%04d%02d%02d %02d:%02d:%02d]" format " %s,%d.\n", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday,          \
            p->tm_hour, p->tm_min, p->tm_sec, ##__VA_ARGS__, __FILE__, __LINE__);                                              \
  } while (0)
#endif
