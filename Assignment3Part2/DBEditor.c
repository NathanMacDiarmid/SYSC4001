#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void process2() {
    while (1) {
        printf("I am Process 2\n");
        wait(NULL);
        sleep(1);
    }
}

int main() {
    process2();

    return 0;
}
