#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

#define servportno 45566
#define BUFFER_SIZE 256

void *routine_write( );
void *routine_read();

int sockfd;

int main(int argc, char *argv[]) {
    int portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    pthread_t thread_r, thread_w;

    portno = servportno;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr,"ERROR opening socket\n");
        exit(0);
	}

    server = gethostbyname("turing.csce.uark.edu");
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        fprintf(stderr,"ERROR connecting\n");
        exit(0);
	}
    else
        fprintf(stderr,"\nThe communication channel has been connected successfully! Ready to go!\n");

	fprintf(stderr,"\nWelcome to the Chat Center\n");
	fprintf(stderr,"There are three chat room available and you can join anyone freely.\n");
	fprintf(stderr,"To operate properly, please follow three following formats when you join, exit, or send message. Otherwise, your request will be rejected by the server\n");
	fprintf(stderr,"1, to join one chat room: join <chat room number> <handler>\n");
	fprintf(stderr,"2, to exit one chat room: exit <chat room number>\n");
	fprintf(stderr,"3, to send one message to one chat room: msg <chat room number> <message>\n");
	fprintf(stderr,"THE COMMANDS, i.e. join, exit, msg, SHOULD BE IN lowcase. Enjoy :)\n\n");

    pthread_create(&thread_w, NULL, routine_write, NULL); // Thread to write to socket
    pthread_create(&thread_r, NULL, routine_read, NULL); // Thread to read to socket

    // Join threads
    pthread_join(thread_w, NULL);
    pthread_join(thread_r, NULL);

	close(sockfd);
    return 0;
}

// Write to socket
void *routine_write() {
    char buffer[BUFFER_SIZE];

    while(1) {
        int n = 0;
        bzero(buffer, sizeof(buffer));
        while((buffer[n++] = getchar()) != '\n'); // Read user input
        write(sockfd, buffer, strlen(buffer));
    }
}

// Read from socket
void *routine_read() {
    char buffer[BUFFER_SIZE];

    while(1) {
        bzero(buffer, sizeof(buffer));
        int n = read(sockfd, buffer, sizeof(buffer));
        
        // Break from loop if the server closes and no messages are read
        if(n <= 0)
            break;

        printf("%s\n", buffer);
    }
}



