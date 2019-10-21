#ifndef SCHED_H_
#define SCHED_H_

#include "../include/procq.h"

#define QUEUE_COUNT 3
#define HIGH_PRIORITY_QUANTA 8


/*Highest priority starts at index 0.*/
typedef struct multi_level_feedback_queue {
    process_queue_t active_queues[QUEUE_COUNT];
    process_queue_t expired_queues[QUEUE_COUNT];
    unsigned int time_quanta[QUEUE_COUNT];
} multi_level_feedback_queue_t;


void init_multi_level_feedback_queue();

#endif
