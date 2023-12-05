#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

// Define a key for the shared memory
#define SHARED_MEM_KEY 12345
#define SEM_KEY 54321
#define SHM_MODE 0666


int main(void) {
    int status;
    pid_t child_pid;
    int shmid;
    char *shared_memory;
    int semid;

    // Create or get the shared memory segment
    shmid = shmget(IPC_PRIVATE, sizeof(char), SHM_MODE);
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

    // Create or get the semaphore
    semid = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | SHM_MODE);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    // Initialize the semaphore value to 1 (mutex)
    semctl(semid, 0, SETVAL, 1);

    printf("Process 1: Starting...\n");

    // Launch Process 2 using fork()
    child_pid = fork();

    if (child_pid == -1) {
        perror("fork");
        exit(1);
    }

    if (child_pid == 0) {
        // This is the child process (Process 2)
        execlp("process2", "process2", NULL);
        perror("exec");
        exit(1);
    } else {
        // This is the parent process (Process 1)
        printf("Process 1: Process 2 is started (PID: %d).\n", child_pid);

        while (1) {
            printf("Process 1: Enter '1' to set the shared variable to 1. Enter '2' to set it to 2. Enter 'x' to exit.\n");

            char input;
            scanf(" %c", &input);

            // Acquire the semaphore
            struct sembuf sem_op = {0, -1, SEM_UNDO};
            semop(semid, &sem_op, 1);

            // Update the shared variable
            *shared_memory = input;

            // Release the semaphore
            sem_op.sem_op = 1;
            semop(semid, &sem_op, 1);

            // Wait for Process 2 to finish
            waitpid(child_pid, &status, 0);

            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                printf("Process 1: Process 2 has exited successfully.\n");
            } else {
                printf("Process 1: Process 2 did not exit normally.\n");
            }

            // Check the shared variable
            if (*shared_memory == 'x') {
                printf("Process 1: Exiting...\n");
                break;
            }
        }

        // Detach the shared memory
        if (shmdt(shared_memory) == -1) {
            perror("shmdt");
            exit(1);
        }

        // Remove the shared memory segment
        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl");
            exit(1);
        }

        // Remove the semaphore
        if (semctl(semid, 0, IPC_RMID) == -1) {
            perror("semctl");
            exit(1);
        }
    }

    return 0;
}





