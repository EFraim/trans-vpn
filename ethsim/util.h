#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>

void ms_sleep(int ms);

/*
 * Returns current time in milliseconds.
 */
uint32_t get_current_time();

#endif
