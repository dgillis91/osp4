#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <errno.h>

#include "../include/proc_table.h"
#include "../include/semutil.h"
#include "../include/shmutil.h"

#define PERM (S_IRUSR | S_IWUSR)


static const uint32_t ONE = 1;

static int semid;
static int shid;

static struct sembuf semlock;
static struct sembuf semunlock;

static pcb_table_t* process_table = NULL;


int init_process_table(int key) {
    setsembuf(&semlock, 0, -1, 0);
    setsembuf(&semunlock, 0, 1, 0);

    shid = shmget(key, sizeof(pcb_table_t), PERM | IPC_CREAT | IPC_EXCL);

    // Couldn't gedt shared memory, and there's a real error.
    if ((shid == -1) && errno != EEXIST) {
        return -1;
    }

    // Already created. Attach. 
    if (shid == -1) {
        // Access
        if ((shid = shmget(key, sizeof(pcb_table_t), PERM)) == -1) {
            return -1;
        }
        // Attach
        if ((process_table = (pcb_table_t*) shmat(shid, NULL, 0)) == (void*)(-1)) {
            return -1;
        }
    } else {
        // Create in the first shmget call. 
        process_table = (pcb_table_t*) shmat(shid, NULL, 0);
        if (process_table == (void*)(-1)) {
            return -1;
        }
        process_table->count_processes_allocated = 0;
        process_table->process_allocation_bitmap = 0;
    }
    semid = initsemset(key, 1, &process_table->ready);
    if (semid == -1) {
        return -1;
    }
    return 1;
}


int destruct_process_table() {
    if (removesem(semid) == -1) {
        return -1;
    }
    if (detachandremove(shid, process_table) == -1) {
        return -1;
    }
    return 1;
}


static unsigned int next_available_pid() {
    uint32_t available_pid_bitmap = ~process_table->process_allocation_bitmap;
    unsigned int first_available_pid = __builtin_ffs(available_pid_bitmap);
    return first_available_pid;
}


static void setbit(unsigned int index) {
    uint32_t bitpos = ONE << index;
    process_table->process_allocation_bitmap |= bitpos;
}


static void unsetbit(unsigned int index) {
    uint32_t bitpos = ONE << index;
    bitpos = ~bitpos;
    process_table->process_allocation_bitmap &= bitpos;
}


/* Returns true if the process buffer has
 * `PROCESS_BUFFER_LENGTH` PCBs in it.
 */
int is_process_buffer_full() {
    if (semop(semid, &semlock, 1) == -1) 
        return -1;
    int is_full = process_table->count_processes_allocated == PROCESS_BUFFER_LENGTH;
    if (semop(semid, &semunlock, 1) == -1)
        return -1;
    return is_full;
}

void deallocate_pid(pcb_t* pcb) {
    reset_pcb(pcb);
    unsetbit(pcb->pid);
    --process_table->count_processes_allocated;
}


/* Find the next available PID, and allocate it.
 * Places the `actual_pid` in the buffer entry
 * for the next pid found.
 * Users should check whether the process buffer
 * is full, first. This is because we also choose
 * to generate process IDs, and things are a tad
 * messy. 
 * Will return -1 if the buffer is full. Again,
 * this is just a safety net.
 */
int allocate_next_pid(pid_t actual_pid) {
    if (semop(semid, &semlock, 1) == -1) 
        return -1;
    if (process_table->count_processes_allocated >= PROCESS_BUFFER_LENGTH) {
        return -1;
    }
    unsigned int next_pid = next_available_pid();
    setbit(next_pid);
    reset_pcb(&process_table->buffer[next_pid]);
    process_table->buffer[next_pid].actual_pid = actual_pid;
    ++process_table->count_processes_allocated;
    if (semop(semid, &semunlock, 1) == -1) 
        return -1;
    return next_pid;
}


int get_abstract_pid_from_actual(pid_t actual_pid) {
    int pid_index = 0;
    if (semop(semid, &semlock, 1) == -1)
        return -1;
    unsigned int i;
    for (i = 0; i < process_table->count_processes_allocated; ++i) {
        if (process_table->buffer[i].actual_pid == actual_pid) {
            pid_index = i;
            break;
        }
    }
    if (semop(semid, &semunlock, 1) == -1)
        return -1;
    if (pid_index) {
        return pid_index;
    } else {
        return -1;
    }
}


pcb_t* get_pcb(unsigned int abstract_pid) {
    return &process_table->buffer[abstract_pid];
}


unsigned int get_process_allocated_count() {
    return process_table->count_processes_allocated;
}


/* Return the PCB of the next process ready to unblock
 * at time `current_time`. Unblock it.
 */
pcb_t* unblock_next_ready(unsigned long current_time) {
    int i;
    // Iterate over the whole table
    for (i = 0; i < get_process_allocated_count(); ++i) {
        // If we find a proc ready to unblock
        if (process_table->buffer[i].wake_time > 0 
            && process_table->buffer[i].wake_time <= current_time) {
            // Unblock it and give aa pointer to the caller.
            process_table->buffer[i].wake_time = 0;
            return &process_table->buffer[i];
        }
    }
    // If we get here, we made it through all, and none
    // were ready to unblock.
    return NULL;
}
