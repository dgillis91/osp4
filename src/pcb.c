#include <stdint.h>

#include "../include/pcb.h"

static const uint32_t ONE = 1;

static pcb_table_t* process_table;

static uint32_t taken_pid_bitmap = 0;
static uint8_t processes_allocated_count = 0;


static unsigned int next_available_pid() {
    uint32_t available_pid_bitmap = ~taken_pid_bitmap;
    unsigned int first_available_pid = __builtin_ffs(available_pid_bitmap);
    return first_available_pid;
}


static void setbit(unsigned int index) {
    uint32_t bitpos = ONE << index;
    taken_pid_bitmap |= taken_pid_bitmap;
}


static void unsetbit(unsigned int index) {
    uint32_t bitpos = ONE << index;
    bitpos = ~bitpos;
    taken_pid_bitmap &= bitpos;
}


/* Returns true if the process buffer has
 * `PROCESS_BUFFER_LENGTH` PCBs in it.
 */
int is_process_buffer_full() {
    return processes_allocated_count == PROCESS_BUFFER_LENGTH;
}


/* Find the next available PID, and allocate it. 
 */
int allocate_next_pid() {
    if (processes_allocated_count >= PROCESS_BUFFER_LENGTH) {
        return -1;
    }
    unsigned int next_pid = next_available_pid();
    setbit(next_pid);
    return next_pid;
}