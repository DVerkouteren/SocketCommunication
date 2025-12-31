
  /* 
    SocketsServer.c
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
        exit(1);
    }

   int main(int argc, char *argv[]) {
        int sockfd, newsockfd, portno, clilen;
        char buffer[256];
        struct sockaddr_in serv_addr, cli_addr;
        int n;
        char username[32];
        char message[512];
        char clientUsername[256];

        if (argc < 2) {
            fprintf(stderr,"ERROR, no port provided\n");
            exit(1);
        }
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) 
           error("ERROR opening socket");
        bzero((char *) &serv_addr, sizeof(serv_addr));
        portno = atoi(argv[1]);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(portno);
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        while(1){
            if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
                error("ERROR on binding");
            listen(sockfd,5);
            clilen = sizeof(cli_addr);
            newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
            if (newsockfd < 0) {
                error("ERROR on accept");
            }
            
            printf("Provide user name: ");
            fflush(stdout);
            fgets(username,32,stdin);
            username[strcspn(username, "\n")] = 0;

            bzero(clientUsername, 32);
            n = read(newsockfd, clientUsername, 31);
            if (n < 0) {
                error("ERROR reading client's username from socket");
            }

            n = write(newsockfd, username, strlen(username));
            if (n < 0) {
                error("ERROR writing username to socket");
            }

            while(1) {
                bzero(buffer,256);
                n = read(newsockfd,buffer,255);
                
                if (n < 0) {
                    error("ERROR reading from socket");
                }

                if (!strcmp(buffer, "quit")) {
                    printf("Other user quit. Exiting now...");
                    exit(0); 
                }

                printf("<%s> %s\n", clientUsername, buffer);

                printf("<%s> ", username); 
                fflush(stdout); 

                bzero(buffer,256); 
                fgets(buffer,255,stdin); 
                buffer[strcspn(buffer, "\n")] = 0;

                n = write(newsockfd,buffer,strlen(buffer)); 
                if (n < 0) {
                    error("ERROR writing to socket");
                }

                if (!strcmp(buffer, "quit")) {
                    printf("Exiting Now...");
                    exit(0); 
                }
            }
        }
        return 0; 
   }