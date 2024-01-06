#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// struct that holds all necessary process info
// cannot change from this
typedef struct {
    int PID;
    int arrivalTime;
    int CPUTime;
    int IOFreq;
    int IODuration;
} ProcessInformation;

// each node on a queue contains these two parameters
struct node {
    ProcessInformation process;
    struct node *next;
};
typedef struct node node;


// each queue contains these two parameters
struct queue {
    node *front;
    node *rear;
    int count;
};
typedef struct queue queue;

// initializes a queue
void initialize(queue *q) {
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}


// checks if the queue is empty
bool isEmpty(queue *q) {
    if (q->front == NULL) {
        return true;
    } else {
        return false;
    }
}

// adds a node to the back of a queue
void enqueue(queue *q, ProcessInformation process) {
    node *temp;
    temp = malloc(sizeof(node));
    temp->process = process;
    temp->next = NULL;

    if (!isEmpty(q)) {
        q->rear->next = temp;
        q->rear = temp;
    } else {
        q->front = q->rear = temp;
    }
    q->count++;
}

// removes a node from the front of a queue
void dequeue(queue *q) {
    node *temp;
    temp = q->front;
    q->front = q->front->next;
    free(temp);
    q->count--;
}

// dequeues a specific process in the queue
void dequeueProcess(queue *q, int targetProcess) {
    if (q->front == NULL) {
        return; // Nothing to dequeue
    }

    // Special case: if the target data is in the first node
    if (q->front->process.PID == targetProcess) {
        node *temp = q->front;
        q->front = q->front->next;
        free(temp);
        return;
    }

    node *current = q->front;
    node *previous = NULL;

    // Traverse the list to find the node with the specified data
    while (current != NULL && current->process.PID != targetProcess) {
        previous = current;
        current = current->next;
    }

    // If the target data is not found, do nothing
    if (current == NULL) {
        return;
    }

    // Update the 'next' pointer of the previous node to skip the node with the specified data
    previous->next = current->next;

    // Free the memory of the node with the specified data
    free(current);
    q->count--;
}

// displays the queue nodes, equivalent to printList
void display(node *head) {
    if (head == NULL) {
        printf("NULL\n");
    } else {
        printf("%d\n", head->process.PID);
        display(head->next);
    }
}

int main() {

FILE *fp;
char row[100];
int count = 0;
int entries = 0;
int position = 0;
char file_name[100];
printf("Enter the file name where your test cases are stored:\n");
scanf("%s", file_name);
strcat(file_name, ".csv");
printf("%s", file_name);
char temp[100];
    fp = fopen(file_name,"r");

    while (!feof(fp))
    
    {
	  fgets(row,100,fp);
        count++;
    }

printf("Count:%d\n", count);
fclose(fp);
count -=1;
    
ProcessInformation processes[count]; 

FILE *fp1;
fp1 = fopen(file_name,"r");
char buffer[100];
fgets(buffer, 100, fp1);

while(!feof(fp1) && position < count){
	fscanf(fp1,"%d,%d,%d,%d,%d",&processes[position].PID, 	&processes[position].arrivalTime, 	&processes[position].CPUTime, &processes[position].IOFreq, 	&processes[position].IODuration);
      position++;
}

for(int i = 0; i < count; i++){
	printf("%d %d %d %d %d\n", processes[i].PID, 	processes[i].arrivalTime, processes[i].CPUTime, 	processes[i].IOFreq, processes[i].IODuration);
}
    
    // a list of all the processes that need to be stored
    // need to read and save processes into a list like this
   // file readable stops here, more initialization
    ProcessInformation CPU;

    queue *readyQueue;
    readyQueue = malloc(sizeof(queue));
    initialize(readyQueue);

    queue *waitingQueue;
    waitingQueue = malloc(sizeof(queue));
    initialize(waitingQueue);

    queue *waitingQueueTimer;
    waitingQueueTimer = malloc(sizeof(queue));
    initialize(waitingQueueTimer);

    // this is only used for debugging so we can vizualize output
    // can take out later or leave in, depending on TA
    queue *finishedQueue;
    finishedQueue = malloc(sizeof(queue));
    initialize(finishedQueue);

    int clock = 0;
    int CPURunTime = 0;
	CPU.PID = 0;

    printf("STARTING LOOP\n\n");

    // will change this when complete, only put 15 to end program without infinite loop
    while (finishedQueue->count != count) {

        // checks if a process arrival time on 
        for (int i = 0; i < count; i++) {
            if (processes[i].arrivalTime == clock) {
                enqueue(readyQueue, processes[i]);
            }
        }

        printf("In ready queue:\n");
        display(readyQueue->front);

        // runs process on CPU
        if (CPU.PID == 0) {
            // checks if CPU has a process already
            // adds one to the CPU if it doesn't
            if (!isEmpty(readyQueue)) {
                // if not, add the first element in readyQueue
                // and begin running, decrementing its CPUTime
                CPU = readyQueue->front->process;
                dequeue(readyQueue);
                CPU.CPUTime--;
                CPURunTime++;
                printf("In CPU\n");
                printf("PID: %d\n", CPU.PID);
                printf("CPUTime: %d\n", CPU.CPUTime);
            } 
        } else {
            // otherwise, decrement CPUTime of the
            // process that is already in the CPU
            CPU.CPUTime--;
            CPURunTime++;
            printf("In CPU\n");
            printf("PID: %d\n", CPU.PID);
            printf("CPUTime: %d\n", CPU.CPUTime);
        }
        // if at any point the CPUTime of a process is 0
        // its finished and send it to the finishedQueue
        if (CPU.CPUTime == 0 && CPU.PID != 0) {
            enqueue(finishedQueue, CPU);
            CPURunTime = 0; 
            CPU.PID = 0;
        } else if (CPU.IOFreq == CPURunTime) {
            // otherwise, if the amount of time the process
            // has run is the same as the call to the IO device
            // add it to the waitingQueue queues
            enqueue(waitingQueue, CPU);
            enqueue(waitingQueueTimer, CPU);
            CPURunTime = 0;
            CPU.PID = 0;
        }

        // runs waiting queue decrementation
        // if waitingQueue isn't empty
        if (waitingQueue->front != NULL) {
            printf("In waiting queue\n");
            // Uses waitingQueueTimer to decrement waiting time because
            // the waiting time needs to be reset once it leaves the waiting
            // queue. When the process it sent back to the readyQueue, the
            // equivalent process in waitingQueue is sent rather than the process
            // found in waitingQueueTimer.
            node *timer = waitingQueueTimer->front;
            node *process = waitingQueue->front;
            // iterate through the whole queue
            while (timer != NULL) {
                printf("PID: %d\n", timer->process.PID);
                printf("PID Waiting Time: %d\n", timer->process.IODuration);
                // if the time the process has to wait in IO is 0, send back
                // to readyQueue and dequeue it from both waiting queues
                if (timer->process.IODuration == 0) {
                    enqueue(readyQueue, process->process);
                    dequeueProcess(waitingQueue, timer->process.PID);
                    dequeueProcess(waitingQueueTimer, timer->process.PID);
                } else {
                    // otherwise, decrement the waitingQueueTimer process
                    timer->process.IODuration--;
                }
                // go to the next process in the list
                timer = timer->next;
                process = process->next;
            }
        }

        clock++;
        printf("CLOCK INCREMENTED\n");
    }

    if (finishedQueue != NULL) {
        printf("Finished queue\n");
        display(finishedQueue->front);
    }
 
    return 0;
}

