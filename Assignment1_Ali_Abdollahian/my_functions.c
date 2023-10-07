//
//  my_functions.c
//  Assignment1 SYSC4001
// Ali_Abdollahian #101229396
// Joshua Makinde  #101193924
//
//  Created on 2023-10-07.
//
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "my_functions.h"

#define QUANTUM 3

/**
 * Reads data from a CSV file, creates PCBs, and stores them in an array.
 *
 * This function opens the specified CSV file, reads its contents, creates PCBs for each row,
 * and stores them in the 'pcbArray' array. The expected CSV file format should have five columns,
 * separated by commas, corresponding to the fields of a PCB structure: Pid, ArrivalTime,
 * CPUTime, IOFrequency, and IODuration.
 *
 * @param filename The name of the CSV file to be read.
 * @param pcbArray An array of PCB structures where each created PCB will be stored.
 * @param maxRecords The maximum number of records to read from the CSV file and create PCBs.
 *
 * @return The number of records successfully read from the CSV file and created as PCBs.
 *         Returns -1 in case of any error, such as a file open failure or incorrect file format.
 */
int readCSV(const char *filename, PCB_t *pcbArray, int maxRecords) {
    FILE *file;
    file = fopen(filename, "r");
    
    if (file == NULL) {
        printf("Error opening file.\n");
        return -1;
    }
    
    int records = 0;
    
    char header[100];
    fgets(header, sizeof(header), file); // Read and discard the header line

    while (records < maxRecords && fscanf(file, "%d,%d,%d,%d,%d\n",
            &pcbArray[records].Pid,
            &pcbArray[records].ArrivalTime,
            &pcbArray[records].CPUTime,
            &pcbArray[records].IOFrequency,
            &pcbArray[records].IODuration) == 5) {
        // Create PCB using the extracted data
        PCB_t *pcb = createPCB(pcbArray[records].Pid, pcbArray[records].ArrivalTime,
                               pcbArray[records].CPUTime, pcbArray[records].IOFrequency,
                               pcbArray[records].IODuration);
        
        if (pcb == NULL) {
            printf("Error creating PCB for record %d\n", records);
            fclose(file);
            return -1;
        }
        
        // Store the created PCB in the array
        pcbArray[records] = *pcb;
        records++;
    }

    fclose(file);
    printf("\n%d records read and created as PCBs.\n\n", records);
    return records;
}

/**
 * Allocates memory and initializes a new queue. The allocated queue is initially empty.
 *
 * @return A pointer to the newly allocated queue. Returns NULL in case of memory allocation failure.
 */
queue_t *alloc_queue(void){
    queue_t *queue = malloc(sizeof(queue_t));
    assert(queue != NULL);
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    return queue;
}

/**
 * Enqueues a PCB at the rear of the queue pointed to by parameter 'queue'.
 * If the queue is empty, it sets both the front and rear pointers to the new PCB.
 *
 * @param queue A pointer to the queue to which the PCB will be enqueued.
 * @param pcb A pointer to the PCB to be enqueued.
 */
void enqueue(queue_t *queue, PCB_t *pcb) {

    if (queue->size == 0) {
        // If the queue is empty, add the new PCB at the front
        queue->front = pcb;
    } else {
        // If the queue has other elements, add the new PCB at the end
        queue->rear->next = pcb;
    }

    // Set the rear pointer to point to the new end of the queue
    queue->rear = pcb;
    queue->size += 1;
}


/**
 * Dequeues and removes the PCB at the front of the queue pointed to by parameter 'queue'.
 * If the queue is not empty, it removes the front PCB, frees its memory, and updates the queue's front and size.
 *
 * @param queue A pointer to the queue from which to dequeue.
 */
void dequeue(queue_t *queue, _Bool deallocated) {
    if (queue->size == 0) {
        // Queue is empty; nothing to dequeue.
        return;
    }

    PCB_t *pcb_to_remove = queue->front;

    if (deallocated) {
        // Deallocate memory if specified
        if (pcb_to_remove != NULL) {
            queue->front = queue->front->next;
            pcb_to_remove = NULL;
        }
    } else {
        // Update front pointer without deallocating memory
        queue->front = queue->front->next;
        
    }

    if (queue->front == NULL) {
        // Queue is now empty; update rear pointer.
        queue->rear = NULL;
    }

    queue->size--;
}

/**
 * Creates a new Process Control Block (PCB) with the specified parameters.
 *
 * This function allocates memory for a new PCB structure, initializes its fields with
 * the provided values, and sets the 'next' pointer to NULL, indicating that the PCB
 * is not part of a linked list.
 *
 * @param Pid The Process ID (Pid) of the PCB.
 * @param ArrivalTime The arrival time of the process in the system.
 * @param CPUTime The CPU time required for the process.
 * @param IOFrequency The frequency of Input/Output (I/O) operations.
 * @param IODuration The duration of each Input/Output (I/O) operation.
 *
 * @return A pointer to the newly created PCB structure. Returns NULL in case of memory
 *         allocation failure.
 */
PCB_t *createPCB(int Pid, int ArrivalTime, int CPUTime, int IOFrequency, int IODuration) {
    
    PCB_t *pcb = malloc(sizeof(PCB_t));
    assert(pcb != NULL);
    
    
    pcb->Pid = Pid;
    pcb->ArrivalTime = ArrivalTime;
    pcb->runningTime = 0;
    pcb->remainingCPUtime = CPUTime;
    pcb->CPUTime= CPUTime;
    pcb->waitingTime = 0;
    pcb->IOFrequency = IOFrequency;
    pcb->IODuration = IODuration;

    pcb->next = NULL;
    
    return pcb;
}

/**
 * Retrieves a pointer to the PCB (Process Control Block) at the front of the queue.
 *
 * This function returns a pointer to the PCB at the front of the specified queue
 * without removing the PCB from the queue.
 *
 * @param queue A pointer to the queue from which to retrieve the front PCB.
 * @return A pointer to the PCB at the front of the queue. Returns NULL if the queue
 *         is empty.
 */
PCB_t *front(queue_t *queue){
    return queue->front;
}

/**
 * Creates a new text file with the given filename and writes a predefined header
 * to it.
 *
 * This function creates a new text file with the specified filename and writes a
 * predefined header line to the file. If the file already exists, its contents
 * will be overwritten.
 *
 * @param filename The name of the file to be created.
 * @return 0 on success, 1 on failure to open the file.
 */
int createOutPutFileWithHeader(const char *filename) {
    //const char *header = "Time,PID,Old_State,New_State"; // Predefined header
    
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        perror("Unable to open the file");
        return 1; // Return an error code
    }

//    fprintf(file, "%s\n", header);

    fclose(file);

    printf("Output file created successfully.\n"); // Debugging output

    return 0; // Return 0 to indicate success
}

/**
 * Checks if a given queue is empty.
 *
 * This function determines whether the specified queue is empty by examining its 'size' field.
 * If the 'size' is zero, the queue is considered empty; otherwise, it is not.
 *
 * @param queue A pointer to the queue to be checked for emptiness.
 * @return 'true' if the queue is empty, 'false' otherwise.
 */
_Bool isEmpty(const queue_t *queue) {
    return queue->size == 0;
}

/**
 * Prints the details of a Process Control Block (PCB).
 *
 * This function takes a pointer to a PCB structure and prints its various fields, including
 * PID, Arrival Time, CPU Time, Remaining CPU Time, IO Frequency, IO Duration, State, Old State,
 * and New State. It is used for debugging and displaying information about a PCB.
 *
 * @param pcb A pointer to the PCB structure to be printed.
 */
void printPCB(const PCB_t *pcb) {
    printf("PID: %d\n", pcb->Pid);
    printf("Arrival Time: %d\n", pcb->ArrivalTime);
    printf("CPU Time: %d\n", pcb->CPUTime);
    printf("Remaining CPU Time: %d\n", pcb->remainingCPUtime);
    printf("IO Frequency: %d\n", pcb->IOFrequency);
    printf("IO Duration: %d\n", pcb->IODuration);
    printf("State: %c\n", pcb->state);
    printf("Old State: %c\n", pcb->oldState);
    printf("New State: %c\n", pcb->newState);
}

/**
 * Logs a transition event to a log file.
 *
 * This function records a transition event to a log file specified by 'logFileName'. The transition
 * event includes information such as the transition type, old state, new state, time, and PID.
 * It appends the event to the log file, creating a header if the file is empty.
 *
 * @param transition_type An integer representing the type of transition.
 * @param old_state A pointer to the queue representing the old state.
 * @param new_state A pointer to the queue representing the new state.
 * @param time The timestamp indicating when the transition occurred.
 * @param logFileName The name of the log file where the transition event will be recorded.
 */
void logTransition(int transition_type, queue_t *old_state, queue_t *new_state, int time, const char* logFileName) {
    // Open the log file for appending
    FILE* logFile = fopen(logFileName, "a");
    if (logFile == NULL) {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }

    // Check if the file is empty
    fseek(logFile, 0, SEEK_END);
    long fileSize = ftell(logFile);
    
    // Determine the transition label based on the transition type
    const char* transition_labels[] = {
        "NEW READY",
        "READY RUNNING",
        "RUNNING WAITING",
        "WAITING READY",
        "RUNNING READY",
        "RUNNING TERMINATED"
    };

    if (transition_type < 0 || transition_type >= 6) {
        printf("Incorrect transition identifier\n");
        exit(EXIT_FAILURE);
    }

    // Print the headers only if the file is empty (at the beginning)
    if (fileSize == 0) {
        fprintf(logFile, "Time\tPID\tOldState\tNewState\n");
    }

    // Split the transition label into OldState and NewState
    char oldState[50];
    char newState[50];
    if (sscanf(transition_labels[transition_type], "%s %s", oldState, newState) != 2) {
        printf("Failed to parse transition label\n");
        exit(EXIT_FAILURE);
    }

    // Print the data for each field with tab as separator
    fprintf(logFile, "%d\t%d\t%s\t%s\n", time, old_state->front->Pid, oldState, newState);

    fclose(logFile);
}

/**
 * Frees the memory used by a queue and its contained PCBs.
 *
 * This function deallocates the memory used by the specified queue and its contained PCBs.
 * It iteratively dequeues and frees each PCB in the queue, setting the 'deallocate' flag to
 * true to deallocate PCB memory. Finally, it frees the queue itself.
 *
 * @param queue A pointer to the queue to be freed.
 */
void free_queue(queue_t *queue) {
    while (!isEmpty(queue)) {
        dequeue(queue, true); // true to deallocate PCB memory
    }

    free(queue); // Free the queue itself
}

/**
 * Runs a simulation of process scheduling using the provided PCBs and logs the transitions.
 *
 * This function simulates process scheduling using the provided PCBs and logs the transitions
 * between different states of the processes. It utilizes several queues (new, ready, running,
 * waiting, and terminated) to manage process states and transitions. The simulation continues
 * until all processes are terminated.
 *
 * @param pcbArray An array of PCB structures representing the processes to be scheduled.
 * @param num_processes The number of processes in the 'pcbArray'.
 * @param outputFileName The name of the output log file where transition information is logged.
 */
void runSimulation(PCB_t pcbArray[], int num_processes, const char* outputFileName) {
    // Create queues for different process states...
    queue_t* ready_queue = alloc_queue();
    queue_t* running_queue = alloc_queue();
    queue_t* waiting_queue = alloc_queue();
    queue_t* terminated_queue = alloc_queue();

    // Clear the log file or create it if it doesn't exist
    createOutPutFileWithHeader(outputFileName);

    // Initialize simulation variables
    int Clock = 0;
    int num_terminated = 0;
    int time_quantum = QUANTUM; // Round Robin time quantum

    while (num_terminated < num_processes) {
        // Add processes to the ready queue at their arrival time
        for (int j = 0; j < num_processes; j++) {
            if (pcbArray[j].ArrivalTime == Clock) {
                enqueue(ready_queue, &pcbArray[j]);
            }
        }

        // Transition from running to terminated or waiting or ready (if time quantum expires)
        if (!isEmpty(running_queue)) {
            PCB_t* pcb = front(running_queue);
            pcb->remainingCPUtime--;

            if (pcb->remainingCPUtime == 0) {
                logTransition(5, running_queue, terminated_queue, Clock, outputFileName);
                dequeue(running_queue, true);
                num_terminated++;
            } else if (time_quantum == 0) {
                // Time quantum expired, move to the end of the ready queue (RR behavior)
                logTransition(4, running_queue, ready_queue, Clock, outputFileName);
                dequeue(running_queue, false);
                enqueue(ready_queue, pcb);
                time_quantum = 3; // Reset the time quantum
            }
        }

        // If running queue is empty, fetch the next process in ready queue (FCFS behavior)
        if (isEmpty(running_queue) && !isEmpty(ready_queue)) {
            logTransition(1, ready_queue, running_queue, Clock, outputFileName);
            PCB_t* pcb = front(ready_queue);
            dequeue(ready_queue, false);
            enqueue(running_queue, pcb);
            time_quantum = QUANTUM; // Reset the time quantum for new process
        }

        // Transition from running to waiting
        if (!isEmpty(running_queue)) {
            PCB_t* pcb = front(running_queue);
            pcb->runningTime++;

            if (pcb->runningTime == pcb->IOFrequency) {
                logTransition(2, running_queue, waiting_queue, Clock, outputFileName);
                dequeue(running_queue, false);
                pcb->runningTime = 0; // Reset running time
                enqueue(waiting_queue, pcb);
            }
        }

        // Transition from waiting to ready
        if (!isEmpty(waiting_queue)) {
            PCB_t* pcb = front(waiting_queue);
            pcb->waitingTime++;

            if (pcb->waitingTime == pcb->IODuration) {
                logTransition(3, waiting_queue, ready_queue, Clock, outputFileName);
                dequeue(waiting_queue, false);
                enqueue(ready_queue, pcb);
            }
        }

        // Decrement time quantum
        if (time_quantum > 0) {
            time_quantum--; // Decrement the time quantum
        } else {
            time_quantum = QUANTUM; // Reset the time quantum when it reaches 0
        }

        // Increment the Clock
        Clock++;
    }

    // Free allocated memory
    free_queue(ready_queue);
    free_queue(running_queue);
    free_queue(waiting_queue);
    free_queue(terminated_queue);
}
