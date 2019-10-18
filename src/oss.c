#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "../include/proc_table.h"
#include "../include/pclock.h"
#include "../include/util.h"

#define MAX_TIME_BETWEEN_PROCESSES 100
#define MAX_ALLOWABLE_PROCESSES 100
#define CLOCK_TICK_INCREMENT 2

#define CLOCK_KEY 8675309
#define PROC_TABLE_KEY 8675310


int main(int argc, char* argv[]) {
    int count_processes_generated = 0;
    int clock_init_stat, proc_table_init_stat;
    clock_init_stat = init_clock(CLOCK_KEY);
    if (clock_init_stat == -1) {
        perror("Failed to allocate clock");
        destruct_clock();
        return EXIT_FAILURE;
    }
    proc_table_init_stat = init_process_table(PROC_TABLE_KEY);
    if (proc_table_init_stat == -1) {
        perror("Failed to allocate process table");
        destruct_process_table();
        destruct_clock();
        return EXIT_FAILURE;
    }

    // Seed the generator. Only need to generate from
    // the parent.
    srand(time(NULL));

    while (count_processes_generated < MAX_ALLOWABLE_PROCESSES) {
        unsigned long next_child_generation_time;
        tick_clock(CLOCK_TICK_INCREMENT);
    }

    int wait_stat;
    wait(&wait_stat);

    destruct_clock();
    destruct_process_table();
    return EXIT_SUCCESS;
}
