#ifndef TIMING_H
#define TIMING_H

#include <sys/time.h>
#include <time.h>

suseconds_t get_microsec() {
    struct timeval ret;
    gettimeofday(&ret, NULL);
    return (suseconds_t)(ret.tv_sec * 1000000) + ret.tv_usec;
}

void microsleep(int duration) {
    suseconds_t a = get_microsec();
    while ((get_microsec() - a) < duration) {}
}

#endif