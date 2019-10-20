#include "../include/procq.h"
#include "../include/pcb.h"
#include "../include/proc_table.h"


void init_process_queue(process_queue_t* queue) {
    queue->head = queue->length = 0;
}


int process_queue_add(process_queue_t* add_to, pcb_t* pcb_to_add) {
    if (add_to->length == PROCESS_BUFFER_LENGTH) {
        return -1;
    }
    add_to->queue[add_to->head++] = pcb_to_add;
    ++add_to->length;
    return 1;
}


pcb_t* process_queue_pop(process_queue_t* pop_from) {
    if (pop_from->length == 0) {
        return NULL;
    } else {
        pcb_t* popped;
        popped = pop_from->queue[pop_from->head];
        --pop_from->length;
        --pop_from->head;
        return popped;
    }
}
