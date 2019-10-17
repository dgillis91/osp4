#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/pclock.h"

#define RUN_TIME_MAX 1000000

#define CLOCK_KEY 8675309


int main(int argc, char* argv[]) {

    // Initialize the system clock
    int clock_init_stat = init_clock(CLOCK_KEY);
    fprintf(stderr, "[%d] Clock Time in Child: %u.%u: %lu\n",
            (int) getpid(), get_seconds(), get_nano(), get_total_tick());

    return EXIT_SUCCESS;
}