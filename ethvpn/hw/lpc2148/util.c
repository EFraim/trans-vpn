#include "util.h"

#include <lpc2000/lpc214x.h>
#include <lpc2000/io.h>

#include <uip/clock.h>

void sim_sleep(int ms) {
}

void clock_init(void) {
    T0TCR |= BIT0;
    T0PR = 60000;
}

clock_time_t clock_time(void) {
    return T0TC;
}

int exit_application = 0;
