#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "../include/proc_table.h"
#include "../include/pclock.h"
#include "../include/sched.h"
#include "../include/util.h"

#define MAX_TIME_BETWEEN_PROCESSES 1000
#define MAX_ALLOWABLE_PROCESSES 100
#define CLOCK_TICK_INCREMENT 2

#define CLOCK_KEY 8675309
#define PROC_TABLE_KEY 8675310

#define HIGH_PRIORITY_INDEX 0
#define MEDIUM_PRIORITY_INDEX 1
#define LOW_PRIORITY_INDEX 2


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

    multi_level_feedback_queue_t run_queue;
    init_multi_level_feedback_queue(&run_queue);

    // Seed the generator. Only need to generate from
    // the parent.
    srand(time(NULL));

    unsigned long next_child_generation_time = 0;
    unsigned long time_until_next_generation;
    unsigned long current_tick;
    while ((current_tick = get_total_tick()) <= 10000) {
        // If we are ready to generate a new process.
        if (current_tick >= next_child_generation_time) {
            // If the buffer isn't full
            if (!is_process_buffer_full()) {
                // fork the child
                fprint(stdout, "[OSS %u.%u] forking child.\n", get_seconds(), get_nano());
                pid_t child_pid = fork();
                // In the parent
                if (child_pid) {
                    // Allocate the pid
                    unsigned int abstract_pid;
                    abstract_pid = allocate_next_pid(child_pid);
                    // Get the PCB, and add it to the highest priority.
                    pcb_t* new_proc = get_pcb(abstract_pid);
                    process_queue_add(&run_queue.active_queues[HIGH_PRIORITY_INDEX], new_proc);
                } else {
                    execl("user", "user", NULL);
                }
            }
            time_until_next_generation = rand_below(MAX_TIME_BETWEEN_PROCESSES);
            next_child_generation_time = current_tick + time_until_next_generation;
            fprintf(stdout, "[%u.%u] next child generates at %lu\n",
                    get_seconds(), get_nano(), next_child_generation_time);
        } 

        // If there are processes in the blocked queue which are ready to unblock,
        // iterate until they are all unblocked. 
        pcb_t* unblocked;
        while ((unblocked = unblock_next_ready(get_total_tick())) != NULL) {
            // Add it back to the queue it was alreaady in.
            process_queue_add(&run_queue.active_queues[unblocked->priority], unblocked);
        }

        // If there are no processes allocated
        if (!get_process_allocated_count()) {
            tick_clock(CLOCK_TICK_INCREMENT);
        // Else, we are waiting on a process to finish running
        } else {
        
        }
        tick_clock(CLOCK_TICK_INCREMENT);
    }

    destruct_clock();
    destruct_process_table();
    return EXIT_SUCCESS;
}
