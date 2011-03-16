#include "util.h"

#include <time.h>
#include <sys/time.h>

void ms_sleep(int ms) {
    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = ms * 1000000;
    nanosleep(&t, NULL);
}

uint32_t get_current_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000 + t.tv_usec / 1000;
}

int time_ms_diff(uint32_t t1, uint32_t t2) {
    return (int)t1 - (int)t2;
}
