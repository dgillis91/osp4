#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <time.h>

#include "../include/sharedvals.h"
#include "../include/pclock.h"
#include "../include/util.h"

#define RUN_TIME_MAX 1000000
#define INT_PROBABILITY_OF_TERMINATION 50


typedef struct message_buffer {
    long mtype;
    pid_t child_pid;
    unsigned long time_used;
    unsigned long time_response;
    child_stat_t status;
} message_bufer_t;


int is_terminate() {
    unsigned int draw = rand_below(100);
    if (draw <= INT_PROBABILITY_OF_TERMINATION) {
        return 1;
    } else {
        return 0;
    }
}


int main(int argc, char* argv[]) {
    pid_t me = getpid();
    srand(time(NULL) ^ (me << 16));

    // Initialize the system clock
    int clock_init_stat = init_clock(CLOCK_KEY);
    int msgqid = msgget(MESSAGE_QUEUE_KEY, PERMS);
    if (msgqid == -1) {
        perror("Failed to init q in child");
        return 1;
    }

    message_bufer_t msg;
    if (msgrcv(msgqid, &msg, sizeof(msg), (long) getpid(), 0) == -1) {
        perror("msgrcv");
    }

    fprintf(stderr, "[%d] Clock Time in Child: %u.%u: %lu\n",
            (int) me, get_seconds(), get_nano(), get_total_tick());
    fprintf(stderr, "[%d] is_terminate: %u\n", (int) me, is_terminate());

    msg.mtype = (long) getppid();
    msg.status = STAT_TERMINATE;
    msg.time_used = 10;
    msg.time_response = 42;

    // When we send the message to the parent, use getppid()

    return EXIT_SUCCESS;
}