// Name: Shirley Lin
// Date: 2/16/26
// Assignment: Homework 3 - 3.26

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 50
#define READ_END 0
#define WRITE_END 1

int main() {
    char write_msg[BUFFER_SIZE];
    char read_msg[BUFFER_SIZE];
    int fd_1[2]; // Send & receive original message
    int fd_2[2]; // Send & receive modified message
    pid_t pid;

    // Prompt user to type in a message
    printf("Please type in a message: ");
    fgets(write_msg, BUFFER_SIZE, stdin);

    // Create pipes
    if(pipe(fd_1) == -1 || pipe(fd_2) == -1) {
        printf("Pipe failed.");
        return 1;
    }

    // Fork child process
    pid = fork();
    if(pid < 0) {
        printf("Fork failed.");
        return 1;
    }
    else if(pid == 0) { // Child process
        // Read original message from parent process
        close(fd_1[WRITE_END]);
        read(fd_1[READ_END], read_msg, BUFFER_SIZE);
        printf("child read %s", read_msg);
        close(fd_1[READ_END]);

        // Modify message to reverse case of each character
        for(int i = 0; i < strlen(read_msg); i++) {
            if(isupper(read_msg[i]))
                write_msg[i] = tolower(read_msg[i]);
            else if(islower(read_msg[i]))
                write_msg[i] = toupper(read_msg[i]);
        }
        
        // Send modified message to parent process
        close(fd_2[READ_END]);
        write(fd_2[WRITE_END], write_msg, strlen(write_msg) + 1);
        printf("child write %s", write_msg);
        close(fd_2[WRITE_END]);
    }
    else { // Parent process
        // Send original message to child process
        close(fd_1[READ_END]);
        write(fd_1[WRITE_END], write_msg, strlen(write_msg) + 1);
        printf("parent write %s", write_msg);
        close(fd_1[WRITE_END]);
        
        // Read modified message from child process
        close(fd_2[WRITE_END]);
        read(fd_2[READ_END], read_msg, BUFFER_SIZE);
        printf("parent read %s", read_msg);
        close(fd_2[READ_END]);

        wait(NULL);
    }

    return 0;
}