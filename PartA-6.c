#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#define KEY 1234
#define MSG_SIZE  sizeof(struct message)

// Define the structure for the message
struct message {
    long mtype;
    int number;
};

int main() {
    // Create message queue
    int msgid = msgget(KEY, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    // Fork first child process
    pid_t pid1 = fork();

    if (pid1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {  // Child process 1
        struct message msg;

        while (1) {
            msgrcv(msgid, &msg, MSG_SIZE, 1, 0);

            // Process the message
            printf("Process 2 received: %d\n", msg.number);
            msg.number *= 2;

            // Send the message to Process 3
            msg.mtype = 2;
            msgsnd(msgid, &msg, MSG_SIZE, 0);
        }
    }

    // Fork second child process
    pid_t pid2 = fork();

    if (pid2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {  // Child process 2
        struct message msg;

        while (1) {
            msgrcv(msgid, &msg, MSG_SIZE, 2, 0);

            // Process the message
            printf("Process 3 received: %d\n", msg.number);
            msg.number += 1;

            // Display the result
            printf("Result: %d\n", msg.number);
        }
    }

    // Parent process
    struct message msg;

    for (int i = 1; i <= 5; ++i) {
        // Send the message to Process 2
        msg.mtype = 1;
        msg.number = i;
        msgsnd(msgid, &msg, MSG_SIZE, 0);

        // Display the current number
        printf("Process 1 sent: %d\n", msg.number);
        sleep(1);
    }

    // Cleanup
    msgctl(msgid, IPC_RMID, NULL);
    kill(pid1, SIGKILL);
    kill(pid2, SIGKILL);

    return 0;
}
