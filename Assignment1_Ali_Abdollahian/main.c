//
//  main.c
//  Assignment1 SYSC4001
// Ali_Abdollahian #101229396
// Joshua Makinde  #101193924
//
//  Created on 2023-10-07.
//

#include <stdlib.h>
#include <stdio.h>
#include "my_structs.h"
#include "my_functions.h"


int main(void) {
    // Define the maximum number of PCBs
    #define MAX_PCB_COUNT 100

//    if (argc != 2) {
//        printf("Usage: %s <test_file.csv>\n", argv[0]);
//        return 1;
//    }
    const char *testFileName = "/Users/ali/Desktop/test_case_1.csv"; // Get the test file name from the command line

    PCB_t pcbArray[MAX_PCB_COUNT]; // Allocate an array for PCBs
    int num_processes;

    // Read data from the CSV file and create PCBs
    num_processes = readCSV(testFileName, pcbArray, MAX_PCB_COUNT);

    if (num_processes < 0) {
        printf("Error reading CSV file.\n");
        return 1;
    }
    // Run the simulation using the created PCBs
    runSimulation(pcbArray, num_processes, "/Users/ali/Desktop/output.csv");
    return 0;
}
