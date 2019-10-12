/* pclock.h
** Abstract representation of a shared clock.
** We need to create atomic access to the clock (sounds funny). 
** We will have n-many processes. We need to ensure that only
** the parent can modify the clock. All children can read it. 
** Note: shm and sem code inspired by UNIX SYSTEM PROGRAMMING.
*/
#ifndef P_CLOCK_H_
#define P_CLOCK_H_
#include <signal.h>

#define NANO_SEC_IN_SEC 1000000000

/* Abstract representation of a system clock.
*/
typedef struct {
    unsigned int seconds;
    unsigned int nanoseconds;
    unsigned long total_tick;
    sig_atomic_t ready;
} pclock_t;


int init_clock(int);
int destruct_clock(int, int);
int get_copy(pclock_t*);
int tick(int);
pclock_t add(pclock_t, unsigned int);
void add_in_place(pclock_t*, unsigned int);
int is_equal_to_sys_clock(pclock_t);
unsigned int get_seconds();
unsigned int get_nano();
unsigned int get_total_tick();


#endif
