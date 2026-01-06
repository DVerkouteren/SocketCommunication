/* 
    SocketsClient.c
    ECE312-01 Project 1
    AUTHORS: Addie patterson & Dane Verkouteren
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
#include <signal.h>

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
    char server_ip[INET_ADDRSTRLEN];

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
    
    inet_ntop(AF_INET, server->h_addr_list[0], server_ip, sizeof(server_ip));
    printf("Connecting to %s on port %d\n",server_ip, portno);

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
            
        pid_t pid = fork();
            if (pid != 0) { 

                while (1) {
                    bzero(buffer, 256);
                    int n = read(sockfd, buffer, 255);

                    if (n <= 0) {
                        kill(getppid(), SIGTERM);
                        close(sockfd);
                        printf("\nConnection closed.\n");
                        exit(0);
                    }

                    if (!strcmp(buffer, "quit")) {
                        kill(getppid(), SIGTERM);
                        close(sockfd);
                        printf("\nOther user quit.\n");
                        exit(1);
                    }

                    printf("\n<%s> %s\n", otherUser, buffer);
                    printf("<%s> ", username);
                    fflush(stdout);
                }
            } else { 
                    
                while (1) {
                    printf("<%s> ", username);
                    fflush(stdout);

                    bzero(buffer, 256);
                    fgets(buffer, 255, stdin);
                    buffer[strcspn(buffer, "\n")] = 0;

                    write(sockfd, buffer, strlen(buffer));

                    if (!strcmp(buffer, "quit")) {
                        kill(getppid(), SIGTERM);
                        close(sockfd);
                        printf("Exiting...\n");
                        exit(1);
                    }
                }
            }
    } 
    
    return 0; 
}