#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <sys/msg.h>

#include "../include/proc_table.h"
#include "../include/sharedvals.h"
#include "../include/pclock.h"
#include "../include/sched.h"
#include "../include/util.h"

#define MAX_TIME_BETWEEN_PROCESSES 1000
#define MAX_ALLOWABLE_PROCESSES 100
#define CLOCK_TICK_INCREMENT 2

#define HIGH_PRIORITY_INDEX 0
#define MEDIUM_PRIORITY_INDEX 1
#define LOW_PRIORITY_INDEX 2

typedef struct message_buffer {
    long mtype;
    pid_t child_pid;
    unsigned long time_used;
    unsigned long time_response;
    child_stat_t status;
} message_bufer_t;

static int msgqid;

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

    // Initialize the feedback queue.
    multi_level_feedback_queue_t run_queue;
    init_multi_level_feedback_queue(&run_queue);

    // Setup the message queue
    msgqid = msgget(MESSAGE_QUEUE_KEY, IPC_CREAT | PERMS);
    if (msgqid == -1) {
        perror("Failed to initialize parent message queue");
        destruct_process_table();
        destruct_clock();
    }

    // Seed the generator. Only need to generate from
    // the parent.
    srand(time(NULL));

    unsigned long next_child_generation_time = 0;
    unsigned long time_until_next_generation;
    unsigned long current_tick;
    while ((current_tick = get_total_tick()) <= 1000000) {
        // If we are ready to generate a new process.
        if (current_tick >= next_child_generation_time) {
            // If the buffer isn't full
            if (!is_process_buffer_full()) {
                // fork the child
                pid_t child_pid = fork();
                // In the parent
                if (child_pid) {
                    fprintf(stdout, "OSS: [%u.%u] forking child %ld.\n", 
                            get_seconds(), get_nano(), (long) child_pid);
                    // Allocate the pid
                    unsigned int abstract_pid;
                    abstract_pid = allocate_next_pid(child_pid);
                    //fprintf(stderr, "Child Pid: %d; Actual Pid: %d\n", child_pid, abstract_pid);
                    // Get the PCB, and add it to the highest priority.
                    pcb_t* new_proc = get_pcb(abstract_pid);
                    new_proc->init_time = get_total_tick();
                    //fprintf(stderr, "ACTUAL PID: %d\n", (long) new_proc);
                    process_queue_add(&run_queue.active_queues[HIGH_PRIORITY_INDEX], new_proc);
                    //pcb_t* a;
                    //a = process_queue_pop(&run_queue.active_queues[HIGH_PRIORITY_INDEX]);
                    //a->actual_pid;
                    //fprintf(stderr, "rq_len %d\n", run_queue.active_queues[HIGH_PRIORITY_INDEX].queue[1]);
                } else {
                    execl("user", "user", NULL);
                }
            }
            time_until_next_generation = rand_below(MAX_TIME_BETWEEN_PROCESSES);
            next_child_generation_time = current_tick + time_until_next_generation;
        } 

        // If there are processes in the blocked queue which are ready to unblock,
        // iterate until they are all unblocked and assign them baack to their queue.
        pcb_t* unblocked;
        while ((unblocked = unblock_next_ready(get_total_tick())) != NULL) {
            process_queue_add(&run_queue.active_queues[unblocked->priority], unblocked);
        }

        // If there are no processes allocated
        if (!get_process_allocated_count()) {
            tick_clock(CLOCK_TICK_INCREMENT);
        // Else, pull the next process to run, run it, and wait for it. 
        } else {
            process_queue_t* next_act_queue;
            process_queue_t* next_exp_queue;
            int index;
            if (!process_queue_is_empty(&run_queue.active_queues[HIGH_PRIORITY_INDEX])) {
                index = HIGH_PRIORITY_INDEX;
            } else if (!process_queue_is_empty(&run_queue.active_queues[MEDIUM_PRIORITY_INDEX])) {
                index = MEDIUM_PRIORITY_INDEX;
            } else if (!process_queue_is_empty(&run_queue.active_queues[LOW_PRIORITY_INDEX])) {
                index = LOW_PRIORITY_INDEX;
            } else {
                fprintf(stderr, "OSS: Failed to select queue!\n");
                index = -1;
            }
            if (index >= 0) {
                pcb_t* proc;
                message_bufer_t msg;
                fprintf(stderr, "Selected index %d\n", index);
                fprintf(stderr, "OSS: NO SEG FAULT\n");
                next_act_queue = &run_queue.active_queues[index];
                //int j;
                //for (j = 18; j > 0; --j) {
                //    fprintf(stderr, "[-] OSS: q[%d]= %ld \n", 
                //            j, (long) next_act_queue->queue[j]->actual_pid);
                //}
                next_exp_queue = &run_queue.expired_queues[index];
                proc = process_queue_pop(next_act_queue);
                fprintf(stderr, "OSS: NO SEG FAULT 2\n");
                // Send the message
                msg.child_pid = proc->actual_pid;
                msg.mtype = (long) proc->actual_pid;
                fprintf(stderr, "OSS: Sending message to child %ld\n", (long) proc->actual_pid);
                int msg_stat = msgsnd(msgqid, &msg, sizeof(msg), 0);
                if (msg_stat == -1) {
                    perror("Fail to send message to child");
                }
                // Receive the message
                if (msgrcv(msgqid, &msg, sizeof(msg), (long) getpid(), 0) == -1) {
                    perror("Fail to get message from child");
                }
                proc->cpu_tick_count += msg.time_used;
                proc->idle_tick_count += (get_total_tick() - proc->init_time) - proc->cpu_tick_count;
                // Handle the response 0-3
                if (msg.status == STAT_BLOCK) {
                    // TODO: Need time_used and time_response.
                    proc->wake_time = msg.time_response;
                    proc->cpu_tick_count += msg.time_used;
                    proc->state = PCB_STATE_BLOCKED; 
                } else if (msg.status == STAT_TERMINATE) {
                    fprintf(stderr, "OSS: Terminating process with pid %ld at time %lu.%lu\n", 
                            (long) proc->actual_pid, get_seconds(), get_nano());
                    deallocate_pid(proc);
                }
                // If the wait time is such that we move in a q, move to next q
                // Else, expire
                process_queue_add(next_exp_queue, proc);
            }
        }
        tick_clock(CLOCK_TICK_INCREMENT);
    }

    destruct_clock();
    destruct_process_table();

    msgctl(msgqid, IPC_RMID, NULL);

    return EXIT_SUCCESS;
}
