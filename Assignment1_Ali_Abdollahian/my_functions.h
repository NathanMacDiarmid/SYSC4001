//
//  my_functions.h
//  Assignment1 SYSC4001
// Ali_Abdollahian #101229396
// Joshua Makinde  #101193924
//
//  Created on 2023-10-07.
//

#ifndef my_functions_h
#define my_functions_h

#include <stdio.h>
#include "my_structs.h"


int readCSV(const char *filename, PCB_t *pcbArray, int maxRecords);
queue_t *alloc_queue(void);
void enqueue(queue_t *queue, PCB_t *pcb);
void dequeue(queue_t *queue, _Bool deallocated);
PCB_t *createPCB(int Pid, int ArrivalTime, int CPUTime, int IOFrequency, int IODuration);
PCB_t *front(queue_t *queue);
_Bool isEmpty(const queue_t *queue);
void printPCB(const PCB_t *pcb);
void runSimulation(PCB_t pcbArray[], int num_processes, const char* outputFileName);
void logTransition(int transition_type, queue_t *old_state, queue_t *new_state, int time, const char* logFileName);
void free_queue(queue_t *queue);



#endif /*my_functions_h*/
