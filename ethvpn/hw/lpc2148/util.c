#include "util.h"

#include <lpc2000/lpc214x.h>
#include <lpc2000/io.h>

void ms_sleep(int ms) {
}

void init_timer() {
	T0TCR |= BIT0;
}

uint32_t get_current_time() {
	return T0TC;
}

int time_ms_diff(uint32_t t1, uint32_t t2) {
	return ((int)t1 - (int)t2) / 60000;
}

