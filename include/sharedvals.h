#ifndef SHARED_VALS_H_
#define SHARED_VALS_H_

#include <sys/stat.h>

#define CLOCK_KEY 8675309
#define PROC_TABLE_KEY 8675310
#define MESSAGE_QUEUE_KEY 123456789
#define PERMS (S_IRUSR | S_IWUSR)

typedef enum child_stat {
    STAT_TERMINATE,
    STAT_TERMINATE_TIME_QUANTUM,
    STAT_BLOCK,
    STAT_EARLY_PREEMPT
} child_stat_t;

#endif
