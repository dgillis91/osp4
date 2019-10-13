#ifndef PCB_H_
#define PCB_H_

#include <stdint.h>
#include <unistd.h>

#define PROCESS_BUFFER_LENGTH 18

typedef enum process_state {
    NEW, READY, RUNNING, BLOCKED, EXIT
} process_state_t;


typedef struct pcb {
    pid_t actual_pid;
    unsigned int pid;
    process_state_t state;
    unsigned int start_time_tick;
    unsigned int stop_time_tick;
    unsigned int last_burst_time_tick;
    /* Metrics */
    unsigned int cpu_tick_count;
    unsigned int sys_tick_count;
    unsigned int idle_tick_count;
} pcb_t;


typedef struct pcb_table {
    pcb_t buffer[PROCESS_BUFFER_LENGTH];
    u_int32_t processes_allocation_bitmap;
    unsigned int process_allocation_count;
} pcb_table_t;


int initialize_process_table();
int is_process_buffer_full();
int allocate_next_pid();


#endif PCB_H_
