#ifndef PROCQ_H_
#define PROCQ_H_


#include "../include/pcb.h"
#include "../include/proc_table.h"

#define QUEUE_COUNT 3
#define HIGH_PRIORITY_QUANTA 8

typedef struct process_queue {
    pcb_t* queue[PROCESS_BUFFER_LENGTH];
    unsigned int head, tail;
    unsigned int length;
} process_queue_t;


typedef struct multi_level_feedback_queue {
    process_queue_t active_queues[QUEUE_COUNT];
    process_queue_t expired_queues[QUEUE_COUNT];
    unsigned int time_quanta[QUEUE_COUNT];
} multi_level_feedback_queue_t;


void init_process_queue(process_queue_t* queue);
int process_queue_add(process_queue_t* add_to, pcb_t* pcb_to_add);
pcb_t* process_queue_pop(process_queue_t* pop_from);
int process_queue_is_full(process_queue_t* queue);
int process_queue_is_empty(process_queue_t* queue);


void init_multi_level_feedback_queue();

#endif
