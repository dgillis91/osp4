#ifndef PROC_TABLE_H
#define PROC_TABLE_H

#include <stdint.h>
#include <signal.h>

#include "../include/pcb.h"

#define PROCESS_BUFFER_LENGTH 18

typedef struct pcb_table {
    pcb_t buffer[PROCESS_BUFFER_LENGTH];
    u_int32_t process_allocation_bitmap;
    unsigned int count_processes_allocated;
    sig_atomic_t ready;
} pcb_table_t;


int init_process_table(int key);
int destruct_process_table();
int is_process_buffer_full();
void deallocate_pid(pcb_t* pcb);
int allocate_next_pid(pid_t actual_pid);
int get_abstract_pid_from_actual(pid_t actual_pid);
pcb_t* get_pcb(unsigned int abstract_pid);
unsigned int get_blocked_process_count();
unsigned int get_process_allocated_count();
pcb_t* unblock_next_ready(unsigned long current_time);

#endif
