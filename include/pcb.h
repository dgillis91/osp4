#ifndef PCB_H_
#define PCB_H_

#include <stdint.h>
#include <unistd.h>

typedef enum process_state {
    PCB_STATE_NEW, PCB_STATE_READY, 
    PCB_STATE_RUNNING, PCB_STATE_BLOCKED, 
    PCB_STATE_EXIT
} process_state_t;


typedef enum process_priority {
    PCB_PRIORITY_HIGH, 
    PCB_PRIORITY_MEDIUM, 
    PCB_PRIORITY_LOW
} process_priority_t;


typedef struct pcb {
    pid_t actual_pid;
    unsigned int pid;
    process_state_t state;
    process_priority_t priority;
    /* Times */
    unsigned int init_time;
    unsigned int start_time_tick;
    unsigned int stop_time_tick;
    unsigned int wake_time;
    unsigned int last_burst_time_tick;
    /* Metrics */
    unsigned int cpu_tick_count;
    unsigned int sys_tick_count;
    unsigned int idle_tick_count;
} pcb_t;


void reset_pcb(pcb_t* _pcb);

#endif 
