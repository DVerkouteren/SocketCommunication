/* 
    SocketsClient.c
    ECE312-01 Project 1
    AUTHORS: Dane Verkouteren & Addie Patterson
    DESCRIPTION: 
*/

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <strings.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netdb.h>

void error(char *msg) { 
    perror(msg); 
    exit(0); 
} 

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256]; 
    char username[32]; 
    char otherUser[32]; 

    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]); 
        exit(0); 
    }
    portno = atoi(argv[2]); 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 

    if (sockfd < 0) {
        error("ERROR opening socket"); 
    }
    server = gethostbyname(argv[1]); 
        
    if (server == NULL) { 
        fprintf(stderr,"ERROR, no such host\n"); 
        exit(0); 
    } 
        
    bzero((char *) &serv_addr, sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET; 
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno); 
    printf("Waiting for connection . . .\n"); 

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }

    while(1) { 
        printf("Provide user name: "); 
        fflush(stdout); 
        fgets(username,32,stdin); 
        username[strcspn(username, "\n")] = 0; 

        n = write(sockfd, username, strlen(username)); 
        if (n < 0) {
            error("ERROR writing username to socket");
        } 

        bzero(otherUser, 32);
        n = read(sockfd, otherUser, 31);
        if (n < 0) {
            error("ERROR reading server's username from socket");
        } 
            
        while(1) { 
            printf("<%s> ", username); 
            fflush(stdout); 

            bzero(buffer,256); 
            fgets(buffer,255,stdin); 
            buffer[strcspn(buffer, "\n")] = 0;

            n = write(sockfd,buffer,strlen(buffer)); 
            if (n < 0) {
                error("ERROR writing to socket");
            }

            if (!strcmp(buffer, "quit")) {
                printf("Exiting Now...");
                exit(0); 
            }

            bzero(buffer,256); 
            n = read(sockfd,buffer,255); 
            if (n < 0) {
                error("ERROR reading from socket");
            } 

            if (!strcmp(buffer, "quit")) {
                printf("Other user quit. Exiting now...");
                exit(0); 
            }

            printf("<%s> %s\n", otherUser, buffer); 
        } 
    } 
    return 0; 
}