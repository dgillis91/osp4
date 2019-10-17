#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <limits.h>

#include "../include/proc_table.h"
#include "../include/pclock.h"

#define CLOCK_KEY 8675309
#define PROC_TABLE_KEY 8675310


int main(int argc, char* argv[]) {
    int clock_init_stat;
    clock_init_stat = init_clock(CLOCK_KEY);
    if (clock_init_stat == -1) {
        perror("Failed to allocate clock");
        destruct_clock();
        return EXIT_FAILURE;
    }

    unsigned long i;
    for (i = 0; i < NANO_SEC_IN_SEC * 20; i += 100000000) {
        tick_clock(100000000);
        printf("%u.%u: %lu\n", get_seconds(), get_nano(), get_total_tick());
    }

    pid_t child_pid = fork();
    if (child_pid == 0) {
        execl("user", "user", NULL);
    }

    int wait_stat;
    wait(&wait_stat);

    destruct_clock();
    return EXIT_SUCCESS;
}
