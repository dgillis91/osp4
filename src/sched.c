#include "../include/sched.h"

static multi_level_feedback_queue_t feedback_queue;


void init_multi_level_feedback_queue() {
    feedback_queue.time_quanta[0] = HIGH_PRIORITY_QUANTA;
    int i;
    for (i = 1; i < QUEUE_COUNT; ++i) {
        feedback_queue.time_quanta[i] = feedback_queue.time_quanta[i - 1] / 2;
    }
}
