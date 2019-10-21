#include "../include/sched.h"
#include "../include/procq.h"
#include "../include/pcb.h"


void init_multi_level_feedback_queue(multi_level_feedback_queue_t* q) {
    q->time_quanta[0] = HIGH_PRIORITY_QUANTA;
    int i;
    for (i = 1; i < QUEUE_COUNT; ++i) {
        q->time_quanta[i] = q->time_quanta[i - 1] / 2;
    }
}
