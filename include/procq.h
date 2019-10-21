#ifndef PROCQ_H_
#define PROCQ_H_


#include "../include/pcb.h"
#include "../include/proc_table.h"

typedef struct process_queue {
    pcb_t* queue[PROCESS_BUFFER_LENGTH];
    unsigned int head, tail;
    unsigned int length;
} process_queue_t;


void init_process_queue(process_queue_t* queue);
int process_queue_add(process_queue_t* add_to, pcb_t* pcb_to_add);
pcb_t* process_queue_pop(process_queue_t* pop_from);
int process_queue_is_full(process_queue_t* queue);
int process_queue_is_empty(process_queue_t* queue);

#endif
