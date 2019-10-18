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
int allocate_next_pid();

#endif
