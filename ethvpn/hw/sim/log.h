#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>

#define LOG_LEVEL_FATAL     0
#define LOG_LEVEL_ERROR     1
#define LOG_LEVEL_WARNING   2
#define LOG_LEVEL_INFO      3
#define LOG_LEVEL_DEBUG     4

#ifndef LOG_LEVEL
#define LOG_LEVEL           LOG_LEVEL_ERROR
#endif

#define LOG_MESSAGE(msg, prefix, args...)   printf(prefix msg "%s", ##args, "\n")

#define LOG_FATAL(msg, args...) \
    if (LOG_LEVEL >= LOG_LEVEL_FATAL)   LOG_MESSAGE(msg, "FATAL:   ", ##args)
#define LOG_ERROR(msg, args...) \
    if (LOG_LEVEL >= LOG_LEVEL_ERROR)   LOG_MESSAGE(msg, "ERROR:   ", ##args)
#define LOG_WARNING(msg, args...) \
    if (LOG_LEVEL >= LOG_LEVEL_WARNING) LOG_MESSAGE(msg, "WARNING: ", ##args)
#define LOG_INFO(msg, args...) \
    if (LOG_LEVEL >= LOG_LEVEL_INFO)    LOG_MESSAGE(msg, "INFO:    ", ##args)
#define LOG_DEBUG(msg, args...) \
    if (LOG_LEVEL >= LOG_LEVEL_DEBUG)   LOG_MESSAGE(msg, "DEBUG:   ", ##args)

#endif
