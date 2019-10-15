#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <limits.h>

#include "../include/proc_table.h"
#include "../include/pclock.h"

#define CLOCK_KEY 8675309
#define PROC_TABLE_KEY 8675310


static int clock_shid;


int main(int argc, char* argv[]) {
    clock_shid = init_clock(CLOCK_KEY);
    if (clock_shid == -1) {
        perror("Failed to allocate clock");
        destruct_clock(clock_shid);
        return EXIT_FAILURE;
    }

    unsigned long i;
    for (i = 0; i < NANO_SEC_IN_SEC * 20; i += 100000000) {
        tick_clock(100000000);
        printf("%u.%u: %lu\n", get_seconds(), get_nano(), get_total_tick());
    }

    destruct_clock(clock_shid);
    return EXIT_SUCCESS;
}
