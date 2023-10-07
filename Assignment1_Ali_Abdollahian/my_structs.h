//
//  my_structs.h
//  Assignment1 SYSC4001
// Ali_Abdollahian #101229396
// Joshua Makinde  #101193924
//
//  Created on 2023-10-07.
//

#ifndef my_structs_h
#define my_structs_h

typedef struct pcb{ //declaring the PCB struct as a linkedList
    int Pid;
    int ArrivalTime;
    int CPUTime;
    int remainingCPUtime;
    int runningTime;
    int IOFrequency;
    int IODuration;
    int waitingTime;
    char state;
    char oldState;
    char newState;
    struct pcb *next;
} PCB_t;

typedef struct { //Declaring output information as a struct
    int Time;
    int Pid;
    char oldState;
    char newState;
}OutPut_logs;

typedef struct { //Queue struct holds a pointer to the PCBs
    PCB_t *front;
    PCB_t *rear;
    int size;
}queue_t;

#endif /* my_structs_h */
