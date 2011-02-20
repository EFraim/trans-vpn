#include "util.h"

#include <time.h>

void ms_sleep(int ms) {
    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = ms * 1000000;
    nanosleep(&t, NULL);
}
