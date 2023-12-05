#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

// Define the same key used in process1.c
#define SHARED_MEM_KEY 12345
#define SEM_KEY 54321
#define SHM_MODE 0666


int main(void) {
    int shmid;
    char *shared_memory;
    int semid;

    // Get the existing shared memory segment
    shmid = shmget(SHARED_MEM_KEY, sizeof(char), SHM_MODE);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    // Attach the shared memory to the process's address space
    shared_memory = shmat(shmid, NULL, 0);
    if (shared_memory == (char *)-1) {
        perror("shmat");
        exit(1);
    }

    // Get the existing semaphore
    semid = semget(SEM_KEY, 1, SHM_MODE);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    while (1) {
        // Acquire the semaphore
        struct sembuf sem_op = {0, -1, SEM_UNDO};
        semop(semid, &sem_op, 1);

        // Check the shared variable
        if (*shared_memory == '1') {
            printf("Process 2: Received '1'.\n");
        } else if (*shared_memory == '2') {
            printf("Process 2: Received '2'.\n");
        } else if (*shared_memory == 'x') {
            printf("Process 2: Received 'x'. Exiting...\n");

            // Release the semaphore
            sem_op.sem_op = 1;
            semop(semid, &sem_op, 1);

            break;
        }

        // Release the semaphore
        sem_op.sem_op = 1;
        semop(semid, &sem_op, 1);

        // Wait for an input
        printf("Process 2: Press '2' to display the message again. Press 'x' to finish.\n");
        char input;
        scanf(" %c", &input);

        // Acquire the semaphore
        sem_op.sem_op = -1;
        semop(semid, &sem_op, 1);

        // Update the shared variable
        *shared_memory = input;

        // Release the semaphore
        sem_op.sem_op = 1;
        semop(semid, &sem_op, 1);

        if (input == 'x') {
            // Break and exit
            break;
        }
    }

    // Detach the shared memory
    if (shmdt(shared_memory) == -1) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}

