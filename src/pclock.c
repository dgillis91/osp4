#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include "../include/pclock.h"
#include "../include/shmutil.h"
#include "../include/semutil.h"

#define PERM (S_IRUSR | S_IWUSR)

// TODO: A better design would be to always have sec and nano derived. Just use total tick.
static int semid;
static struct sembuf semlock;
static struct sembuf semunlock;
static pclock_t* system_clock;


int init_clock(int key) {
    int shid;

    setsembuf(&semlock, 0, -1, 0);
    setsembuf(&semunlock, 0, 1, 0);

    shid = shmget(key, sizeof(pclock_t), PERM | IPC_CREAT | IPC_EXCL);

    // Couldn't gedt shared memory, and there's a real error.
    if ((shid == -1) && errno != EEXIST) {
        return -1;
    }

    // Already created. Attach. 
    if (shid == -1) {
        // Access
        if ((shid = shmget(key, sizeof(pclock_t), PERM)) == -1) {
            return -1;
        }
        // Attach
        if ((system_clock = (pclock_t*) shmat(shid, NULL, 0)) == (void*)(-1)) {
            return -1;
        }
    } else {
        // Create in the first shmget call. 
        system_clock = (pclock_t*) shmat(shid, NULL, 0);
        if (system_clock == (void*)(-1)) {
            return -1;
        }
        system_clock->nanoseconds = 0;
        system_clock->total_tick = 0;
        system_clock->seconds = 0;
    }
    semid = initsemset(key, 1, &system_clock->ready);
    if (semid == -1) {
        return -1;
    }
    return shid;
}


int destruct_clock(int key, int shid) {
    if (removesem(semid) == -1) {
        return -1;
    }
    if (detachandremove(shid, system_clock) == -1) {
        return -1;
    }
    return 1;
}


int get_copy(pclock_t* copy) {
    if (semop(semid, &semlock, 1) == -1) 
        return -1;
    copy->total_tick = system_clock->total_tick;
    copy->nanoseconds = system_clock->nanoseconds;
    copy->seconds = system_clock->seconds;
    if (semop(semid, &semunlock, 1) == -1)
        return -1;
    return 1;
}


int tick(int nanoseconds) {
    /* Increment the system clock by `nanoseconds`.
    ** Method synchronizes access to `system_clock`.
    */
    if (semop(semid, &semlock, 1) == -1)
        return -1;
    add_in_place(system_clock, nanoseconds);
    if (semop(semid, &semunlock, 1) == -1) 
        return -1;
    return 0;
}


// TODO: These clock methods should have more informative names.
pclock_t add(pclock_t clock, unsigned int nanoseconds) {
    add_in_place(&clock, nanoseconds);
    return clock;
}


void add_in_place(pclock_t* clock, unsigned int nanoseconds) {
    /* Add `nanoseconds` to `clock`. Addition done in place.
    ** No regard given to synchronization. Methods acting on
    ** `clock` should work with `clock->readyp` externally.
    */
    clock->total_tick += nanoseconds;
    clock->nanoseconds = clock->total_tick % NANO_SEC_IN_SEC;
    clock->seconds = clock->total_tick / NANO_SEC_IN_SEC;
}


int is_equal_to_sys_clock(pclock_t clock) {
    if (semop(semid, &semlock, 1) == -1) 
        return -1;
    int is_equal = clock.total_tick == system_clock->total_tick;
    if (semop(semid, &semunlock, 1) == -1)
        return -1;
    return is_equal;
}


unsigned int get_seconds() {
    if (semop(semid, &semlock, 1) == -1)
        return 0;
    unsigned int s = system_clock->seconds;
    if (semop(semid, &semunlock, 1) == -1)
        return 0;
    return s;
}


unsigned int get_nano() {
    if (semop(semid, &semlock, 1) == -1) {
        return 0;
    }
    unsigned int n = system_clock->nanoseconds;
    if (semop(semid, &semunlock, 1) == -1) {
        return 0;
    }
    return n;
}


unsigned int get_total_tick() {
    if (semop(semid, &semlock, 1) == -1) {
        return 0;
    }
    unsigned int t = system_clock->total_tick;
    if (semop(semid, &semunlock, 1) == -1) {
        return 0;
    }
    return t;
}
