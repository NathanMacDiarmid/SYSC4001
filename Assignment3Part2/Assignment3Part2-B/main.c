#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int accountNo;
    int accountPin;
    double funds;
} Account;

Account accounts[3];

int sharedAccountNo, sharedPin, pinCount, sharedAccountCmd, accountIndex = 0;
double withdrawAmount = 0;
bool accountAccess = false;
// initializes the mutex to be used as the lock
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// initializes the condition to be used as the wait checker
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

/**
 * @brief Read account information from a CSV file and populate the 'accounts' array.
 *
 * This function opens a CSV file containing account information in the format:
 * "accountNo,accountPin,funds", and reads each line to populate the 'accounts' array.
 * The function uses the strtok function to tokenize each line based on commas as separators.
 *
 * @note The CSV file should have one line per account, and each line should contain
 * the account number, account pin, and funds separated by commas.
 *
 * @note The 'accounts' array must be pre-declared and have sufficient space for the
 * account information from the file.
 */
void readFile(void) {
    // Open the file for reading
    FILE *file = fopen("DataBase.txt", "r");
    if (file == NULL) {
        // Print an error message and exit if the file couldn't be opened
        perror("Error opening file");
        exit(1);
    }

    // Define an array to store each line read from the file
    char line[100];
    // Initialize the counter for the accounts array
    int i = 0;

    // Read each line from the file using fgets
    // Stop reading if the end of the file is reached or the accounts array is filled
    while (fgets(line, sizeof(line), file) != NULL && i < sizeof(accounts) / sizeof(accounts[0])) {
        // Tokenize the line using commas as separators
        char *token = strtok(line, ",");
        if (token == NULL) {
            // Print an error message and exit if the first token is missing
            fprintf(stderr, "Error reading account information from file\n");
            exit(1);
        }

        // Convert the token to an integer and store it in the accountNo field of the current account
        accounts[i].accountNo = atoi(token);

        // Move to the next token
        token = strtok(NULL, ",");
        if (token == NULL) {
            // Print an error message and exit if the second token is missing
            fprintf(stderr, "Error reading account information from file\n");
            exit(1);
        }

        // Convert the token to an integer and store it in the accountPin field of the current account
        accounts[i].accountPin = atoi(token);

        // Move to the next token
        token = strtok(NULL, ",");
        if (token == NULL) {
            // Print an error message and exit if the third token is missing
            fprintf(stderr, "Error reading account information from file\n");
            exit(1);
        }

        // Convert the token to a double and store it in the funds field of the current account
        accounts[i].funds = atof(token);

        // Increment the counter for the accounts array
        i++;
    }

    // Close the file
    fclose(file);
}



// clears any residuals remaining when performing scanf
void clear_input_buffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF);
}

void *ATM(void *arg) {
    while (true) {
        // if any input is -1 end the program
        if (sharedAccountCmd == -1 || sharedAccountNo == -1 || sharedPin == -1 || withdrawAmount == -1) {
            exit(1);
        }
        // if the user has three incorrect pin attempts, pinCount will be assigned -1
        if (pinCount == -1) {
            printf("account is blocked\n");
            exit(1);
        }
        // lock the mutex
        pthread_mutex_lock(&mutex);
        // cirtical section
        // if a user hasn't logged in
        if (!accountAccess) {
            // once a correct account has been entered, it won't go back in this if statement
            if (pinCount == 0) {
                printf("Please enter an account number: ");
                scanf("%d", &sharedAccountNo);
                clear_input_buffer();
            }
            printf("Please enter a pin: ");
            scanf("%d", &sharedPin);
            clear_input_buffer();
            printf("Account number: %d\n", sharedAccountNo);
            printf("Account pin: %d\n", sharedPin);
            pthread_cond_signal(&condition);
            pthread_mutex_unlock(&mutex);
            sleep(1);
        // user has logged in
        } else {
            printf("Choose one of the following:\n");
            printf("- BALANCE: 1\n");
            printf("- WITHDRAW: 2\n");
            printf("- QUIT: -1\n");
            printf("Your choice: ");
            scanf("%d", &sharedAccountCmd);
            clear_input_buffer();
            sleep(1);
            if (sharedAccountCmd == 2) {
                printf("Amount to withdraw: ");
                scanf("%lf", &withdrawAmount);
                clear_input_buffer();
            }
            pthread_cond_signal(&condition);
            pthread_mutex_unlock(&mutex);
            sleep(1);
        }
    }

    pthread_exit(NULL);
}

void *DBServer(void *arg) {
    while (true) {
        // lock the mutex
        pthread_mutex_lock(&mutex);
        // waits for signal from others
        while (sharedAccountNo == 0) {
            pthread_cond_wait(&condition, &mutex);
        }
        // critical section
        // if user hasn't logged in
        if (!accountAccess) {
            printf("Account number recieved: %d\n", sharedAccountNo);
            printf("Account pin recieved: %d\n", sharedPin);
            // if user hasn't failed the pin 3 times
            if (pinCount < 3) {
                for (int i = 0; i < sizeof(accounts)/sizeof(accounts[i]); i++) {
                    // if the account number matches the account number passed
                    if (accounts[i].accountNo == sharedAccountNo) {
                        // if account pin matches account number and pin combo passed
                        // subtracts 1 from each to 'encrypt' the pin
                        if (accounts[i].accountPin - 1 == sharedPin - 1) {
                            printf("OK\n");
                            accountAccess = true;
                            accountIndex = i;
                            pinCount = 0;
                            break;
                        // if pin and account number combo don't match, increment the pinCount
                        // pinCount is the number of failed attempts
                        } else if (accounts[i].accountPin - 1 != sharedPin - 1) {
                            pinCount++;
                            if (pinCount >= 3) {
                                pinCount = -1;
                            } else {
                                printf("PIN_WRONG %d tries left\n", 3 - pinCount);
                            }
                            break;
                        }
                    }
                }
            }
        }
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }

    pthread_exit(NULL);
}

void *DBEditor(void *args) {
    while (true) {
        // lock the mutex
        pthread_mutex_lock(&mutex);
        // wait for signal from both threads
        while (sharedAccountCmd != 2 && sharedAccountCmd != 1 && sharedAccountCmd != -1) {
            pthread_cond_wait(&condition, &mutex);
        }
        // critical section
        // if the command passed by ATM requests balance
        if (sharedAccountCmd == 1) {
            printf("Account balance: %.2f\n", accounts[accountIndex].funds);
        // if the command passed by ATM requests a withdraw
        } else if (sharedAccountCmd == 2) {
            // if the funds in the account are insufficient
            if (accounts[accountIndex].funds - withdrawAmount < 0) {
                printf("NSF\n");
            // funds in the account are suffucient, remove the funds from the account
            } else {
                printf("FUNDS_OK\n");
                printf("Previous balance: %.2f\n", accounts[accountIndex].funds);
                accounts[accountIndex].funds -= withdrawAmount;
                printf("New balance: %.2f\n", accounts[accountIndex].funds);
            }
        }
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    pthread_exit(NULL);
}

int main() {

    // read the file
    readFile();

    // initialize the threads
    pthread_t ATMThread, DBServerThread, DBEditorThread;

    pthread_create(&ATMThread, NULL, ATM, NULL);
    pthread_create(&DBServerThread, NULL, DBServer, NULL);
    pthread_create(&DBEditorThread, NULL, DBEditor, NULL);

    // run each individual thread
    pthread_join(ATMThread, NULL);
    pthread_join(DBServerThread, NULL);
    pthread_join(DBEditorThread, NULL);

    // delete the mutex and condition
    // equivalent of deallocating
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condition);

    return 0;
}
