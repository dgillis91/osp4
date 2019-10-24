#include <sys/msg.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PERMS (S_IRUSR | S_IWUSR)

#define KEY 8675309
#define MAX_CHILD_PROC_COUNT 3

typedef enum action {
    ACTION_TERMINATE, ACTION_RUN
} action_t;

typedef struct message {
    long mtype;
    pid_t child_pid;
    action_t act;
} message_t;


int main(int argc, char* argv[]) {
    pid_t children[MAX_CHILD_PROC_COUNT];
    int msg_q_id = msgget(KEY, IPC_CREAT | PERMS);
    if (msg_q_id == -1) {
        perror("Failed to create message queue");
    }

    int i;
    for (i = 0; i < MAX_CHILD_PROC_COUNT; ++i) {
        pid_t new_child = fork();
        if (!new_child) {
            execl("child", "child", NULL);
            return 0;
        }
        children[i] = new_child;
    }

    for (i = 0; i < MAX_CHILD_PROC_COUNT; ++i) {
        message_t msg;
        msg.mtype = (long) children[i];
        msg.act = ACTION_TERMINATE;
        msg.child_pid = children[i];

        fprintf(stderr, "Sending message to child: %d\n", (int) msg.child_pid);
        if (msgsnd(msg_q_id, &msg, sizeof(msg), 0) == -1) {
            perror("Failed to send message to child");
        }
    }

    int status;
    while (wait(&status) > 0) ;
    msgctl(msg_q_id, IPC_RMID, NULL);
    return 0;
}
