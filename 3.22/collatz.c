// Name: Shirley Lin
// Date: 2/10/26
// Assignment: Homework 2 - 3.22

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define SIZE 4096

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

    const char *name = "lin_collatz"; // Name of shared memory
    int shm_fd; // Shared memory file descriptor
    void *ptr; // Pointer to shared memory object

    pid_t pid;
    pid = fork();

    if(pid < 0) {
        printf("Fork failed.\n");
    }
    else if(pid == 0) { // Child process
        // Create shared memory object
        shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
        ftruncate(shm_fd, SIZE);
        ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

        // Write Collatz sequence to shared memory
        while(num != 1) {
            int n = sprintf(ptr, "%d, ", num);
            ptr += n; // Move pointer so next write appends
            
            if(num % 2 == 0) {
                num = num / 2;
            }
            else {
                num = 3 * num + 1;
            }
        }
        sprintf(ptr, "1\n");
    }
    else { // Parent process
        wait(NULL);
        printf("Child process completed. Printing Collatz sequence.\n");

        // Read from shared memory and print Collatz sequence
        shm_fd = shm_open(name, O_RDONLY, 0666);
        ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
        printf("%s", (char *)ptr);

        // Cleanup shared memory
        shm_unlink(name);
    }

    return 0;
}