#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>

#define SHM_SIZE  sizeof(char)

void process1(char *shared_data, int sem_id) {
    struct sembuf sem_op;

    printf("Process 1 (PID: %d) started\n", getpid());

    sem_op.sem_num = 0;
    sem_op.sem_op = -1; // P operation
    sem_op.sem_flg = 0;

    // Wait for Process 2 to finish
    wait(NULL);

    semop(sem_id, &sem_op, 1); // Acquire the semaphore

    printf("Process 1 received: %c\n", *shared_data);

    sem_op.sem_op = 1; // V operation
    semop(sem_id, &sem_op, 1); // Release the semaphore

    printf("Process 1 exiting\n");
}

void process2(char *shared_data, int sem_id) {
    struct sembuf sem_op;

    printf("Process 2 (PID: %d) started\n", getpid());

    while (1) {
        char input;

        printf("Enter '1' to display the message, '2' to display it again, or 'x' to exit: ");
        scanf(" %c", &input);

        sem_op.sem_num = 0;
        sem_op.sem_op = -1; // P operation
        sem_op.sem_flg = 0;

        semop(sem_id, &sem_op, 1); // Acquire the semaphore

        *shared_data = input;

        sem_op.sem_op = 1; // V operation
        semop(sem_id, &sem_op, 1); // Release the semaphore

        if (input == 'x') {
            printf("Process 2 exiting\n");
            exit(EXIT_SUCCESS);
        }
    }
}

int main() {
    key_t key = ftok(".", 'S'); // Generate a key for the shared memory segment
    int shmid, sem_id;
    char *shared_data;
    union semun sem_arg;

    // Create a shared memory segment
    shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach the shared memory segment
    shared_data = shmat(shmid, NULL, 0);
    if (shared_data == (char *) -1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Create a semaphore
    sem_id = semget(key, 1, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    // Initialize the semaphore value to 1
    sem_arg.val = 1;
    if (semctl(sem_id, 0, SETVAL, sem_arg) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    pid_t pid;

    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process (Process 2)
        process2(shared_data, sem_id);
    } else {
        // Parent process (Process 1)
        process1(shared_data, sem_id);

        // Detach the shared memory segment
        shmdt(shared_data);

        // Remove the shared memory segment
        shmctl(shmid, IPC_RMID, NULL);

        // Remove the semaphore
        if (semctl(sem_id, 0, IPC_RMID, sem_arg) == -1) {
            perror("semctl");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
