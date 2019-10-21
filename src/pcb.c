#include <stdint.h>

#include "../include/pcb.h"

/* Reset to the initial values of `_pcb`.
 */
void reset_pcb(pcb_t* _pcb) {
    _pcb->pid = 0;
    _pcb->state = PCB_STATE_NEW;
    _pcb->priority = PCB_PRIORITY_HIGH;
    _pcb->start_time_tick = 0;
    _pcb->stop_time_tick = 0;
    _pcb->wake_time = 0;
    _pcb->last_burst_time_tick = 0;
    /* Metrics */
    _pcb->cpu_tick_count = 0;
    _pcb->sys_tick_count = 0;
    _pcb->idle_tick_count = 0;
}