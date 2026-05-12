// Name: Shirley Lin
// Date: 2/10/26
// Assignment: Homework 2 - 3.19

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid;
    pid = fork();

    if(pid < 0) {
        printf("Fork failed.");
        return 1;
    } 
    else if(pid == 0) { // Child Process
        return 0; // Immediately end the process
    } 
    else { // Parent Process
        sleep(10); // Continue to run the parent process for 10 seconds
        printf("Process completed.\n");
    }

    return 0;
}