// Name: Shirley Lin
// Date: 2/17/26
// Assignment: Homework 3 - 3.27

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 256
#define READ_END 0
#define WRITE_END 1

int main(int argc, char *argv[]) {
    pid_t pid;
    char file_buffer[BUFFER_SIZE];
    int line_size;
    int fd_pipe[2];
    int fd_file[2];

    // Check validity of command line input
    if(argc != 3) {
        printf("Usage: %s <input filename> <output filename>\n", argv[0]);
        return 1;
    }

    // Create pipe
    if(pipe(fd_pipe) == -1) {
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
        // Open output file in write mode
        fd_file[WRITE_END] = open(argv[2], O_CREAT | O_WRONLY, 0666);
        if(fd_file[WRITE_END] == -1) {
            printf("Could not open file: %s\n", argv[2]);
            return 1;
        }

        // Read contents from pipe and write to output file
        close(fd_pipe[WRITE_END]);
        while((line_size = read(fd_pipe[READ_END], file_buffer, BUFFER_SIZE)) > 0) {
            write(fd_file[WRITE_END], file_buffer, line_size);
        }

        // Close pipe and file
        close(fd_pipe[READ_END]);
        close(fd_file[WRITE_END]);
    }
    else { // Parent process
        // Open input file in read mode
        fd_file[READ_END] = open(argv[1], O_RDONLY);
        if(fd_file[READ_END] == -1) {
            printf("Could not open file: %s\n", argv[1]);
            return 1;
        }

        // Parse file line by line and write to pipe
        close(fd_pipe[READ_END]);
        while((line_size = read(fd_file[READ_END], file_buffer, BUFFER_SIZE)) > 0) {
            write(fd_pipe[WRITE_END], file_buffer, line_size);
            // printf("%s", file_buffer);
        }

        // Close pipe and file
        close(fd_pipe[WRITE_END]);
        close(fd_file[READ_END]);
        wait(NULL);
    }

    return 0;
}