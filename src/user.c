#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "../include/pclock.h"
#include "../include/util.h"

#define RUN_TIME_MAX 1000000
#define INT_PROBABILITY_OF_TERMINATION 50

#define CLOCK_KEY 8675309


int is_terminate() {
    unsigned int draw = rand_below(100);
    if (draw <= INT_PROBABILITY_OF_TERMINATION) {
        return 1;
    } else {
        return 0;
    }
}


int main(int argc, char* argv[]) {
    pid_t me = getpid();
    srand(time(NULL) ^ (me << 16));

    // Initialize the system clock
    int clock_init_stat = init_clock(CLOCK_KEY);
    fprintf(stderr, "[%d] Clock Time in Child: %u.%u: %lu\n",
            (int) me, get_seconds(), get_nano(), get_total_tick());
    fprintf(stderr, "[%d] is_terminate: %u\n", (int) me, is_terminate());

    return EXIT_SUCCESS;
}