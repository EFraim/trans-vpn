#include "util.h"

#include <time.h>
#include <sys/time.h>

#include <uip/clock.h>

void sim_sleep(int ms) {
    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = ms * 1000000;
    nanosleep(&t, NULL);
}

void clock_init(void) {
}

clock_time_t clock_time(void) {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000 + t.tv_usec / 1000;
}

int exit_application = 0;
