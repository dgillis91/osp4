#include "../include/procq.h"
#include "../include/pcb.h"
#include "../include/proc_table.h"


void init_process_queue(process_queue_t* queue) {
    queue->head = queue->length = 0;
    queue->tail = PROCESS_BUFFER_LENGTH - 1;
}


int process_queue_add(process_queue_t* add_to, pcb_t* pcb_to_add) {
    if (process_queue_is_full(add_to)) {
        return -1;
    }
    add_to->tail = (add_to->tail + 1) % PROCESS_BUFFER_LENGTH;
    add_to->queue[add_to->tail] = pcb_to_add;
    ++add_to->length;
    return 1;
}


pcb_t* process_queue_pop(process_queue_t* pop_from) {
    if (process_queue_is_empty(pop_from)) {
        return NULL;
    } else {
        pcb_t* popped;
        popped = pop_from->queue[pop_from->head];
        pop_from->head = (pop_from->head + 1) % PROCESS_BUFFER_LENGTH;
        --pop_from->length;
        return popped;
    }
}


int process_queue_is_full(process_queue_t* queue) {
    return queue->length == PROCESS_BUFFER_LENGTH;
}


int process_queue_is_empty(process_queue_t* queue) {
    return queue->length == 0;
}
