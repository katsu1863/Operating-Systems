// Name: Shirley Lin
// Date: 2/10/26
// Assignment: Homework 2 - 3.21

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // Check validity of command line input
    if(argc != 2) {
        printf("Usage: %s <positive number>\n", argv[0]);
        return 1;
    }

    // Check if the input is a positive integer
    int num = atoi(argv[1]);
    if(num <= 0) {
        printf("Input a positive integer.\n");
        return 1;
    }

    pid_t pid;
    pid = fork();

    if(pid < 0) {
        printf("Fork failed.\n");
        return 1;
    }
    else if(pid == 0) { // Child process
        // Print Collatz sequence
        while(num != 1) {
            printf("%d, ", num);
            
            if(num % 2 == 0) {
                num = num / 2;
            }
            else {
                num = 3 * num + 1;
            }
        }
        printf("1\n");
    }
    else { // Parent process
        wait(NULL);
        printf("Child process completed. Exiting parent process.\n");
    }

    return 0;
}