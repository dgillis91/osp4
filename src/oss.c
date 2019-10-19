#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "../include/proc_table.h"
#include "../include/pclock.h"
#include "../include/util.h"

#define MAX_TIME_BETWEEN_PROCESSES 1000
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

    unsigned long next_child_generation_time = 0;
    unsigned long time_until_next_generation;
    unsigned long current_tick;
    while ((current_tick = get_total_tick()) <= 10000) {
        if (current_tick >= next_child_generation_time) {
            // If the buffer isn't full
            if (!is_process_buffer_full()) {
                // fork the child
                fprint(stdout, "[OSS %u.%u] forking child.\n", get_seconds(), get_nano());
                pid_t child_pid = fork();
                // In the parent
                if (child_pid) {
                    // Allocate the pid
                    allocate_next_pid(child_pid);
                    // Set the variables . . . 
                    // Send a message to the child telling it to run.
                } else {
                    execl("user", "user", NULL);
                }
            }
            time_until_next_generation = rand_below(MAX_TIME_BETWEEN_PROCESSES);
            next_child_generation_time = current_tick + time_until_next_generation;
            fprintf(stdout, "[%u.%u] next child generates at %lu\n",
                    get_seconds(), get_nano(), next_child_generation_time);
        }
        tick_clock(CLOCK_TICK_INCREMENT);
    }

    destruct_clock();
    destruct_process_table();
    return EXIT_SUCCESS;
}
