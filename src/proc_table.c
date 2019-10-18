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


/* Find the next available PID, and allocate it. 
 */
int allocate_next_pid() {
    if (semop(semid, &semlock, 1) == -1) 
        return -1;
    if (process_table->count_processes_allocated >= PROCESS_BUFFER_LENGTH) {
        return -1;
    }
    unsigned int next_pid = next_available_pid();
    setbit(next_pid);
    if (semop(semid, &semlock, 1) == -1) 
        return -1;
    return next_pid;
}