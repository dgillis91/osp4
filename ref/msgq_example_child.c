#include <sys/msg.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PERMS (S_IRUSR | S_IWUSR)

#define KEY 8675309

typedef enum action {
    ACTION_TERMINATE, ACTION_RUN
} action_t;

typedef struct message {
    long mtype;
    pid_t child_pid;
    action_t act;
} message_t;


int main(int argc, char* argv[]) {
    int msg_q_id = msgget(KEY, PERMS);
    if (msg_q_id == -1) {
        perror("Failed to create message queue in child");
        return 1;
    }

    fprintf(stderr, "Waiting in child %d\n", getpid());
    message_t msg;
    if (msgrcv(msg_q_id, &msg, sizeof(msg), (long) getpid(), 0) == -1) {
        perror("msgrcv");
    }
    if (msg.child_pid == getpid()) {
        fprintf(stderr, "Received message in child: %d. Terminating.\n", (int) getpid());
    } else {
        fprintf(stderr, "Some error with the pid.\n");
    }


    msgctl(msg_q_id, IPC_RMID, NULL);
    return 0;
}