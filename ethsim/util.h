#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>

void ms_sleep(int ms);

/*
 * Returns current time in some unit...
 */
uint32_t get_current_time();

/*
 * Returns difference (t1 - t2) in milliseconds.
 * In case t1 < t2, the difference will be negative as well.
 */
int time_ms_diff(uint32_t t1, uint32_t t2);

#endif
