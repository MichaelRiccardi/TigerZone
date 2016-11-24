/* A simple server in the internet domain using TCP
   The port number is passed as an argument
   This version runs forever, forking off a separate
   process for each connection
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void Tournament_Protocol(int);

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;

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
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);

     while (1) {
         newsockfd = accept(sockfd,
               (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0)
             error("ERROR on accept");
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(sockfd);
             Tournament_Protocol(newsockfd);
             exit(0);
         }
         else close(newsockfd);
     } /* end of while */

     close(sockfd);
     return 0; /* we never get here */
}


void Tournament_Protocol (int sock)
{


   int n;
   char buffer[256];
   char pid[] = "PLAYER 1";

   write(sock,"THIS IS SPARTA!",15);

   bzero(buffer,256);
   read(sock,buffer,255);
   write(sock,"HELLO!",6);

   bzero(buffer,256);
   read(sock,buffer,255);
   sprintf(buffer,"WELCOME %s PLEASE WAIT FOR THE NEXT CHALLENGE",pid);
   write(sock,buffer,255);

//CHALLENGE PROTOCOL
   int totalRounds = 5;

   bzero(buffer,256);
   sprintf(buffer,"NEW CHALLENGE <cid> YOU WILL PLAY %d MATCHES", totalRounds);
   write(sock,buffer,255);

   for(int round = 1; round <= totalRounds; round++)
    {
      bzero(buffer,256);
      sprintf(buffer,"BEGIN ROUND %d of %d",round ,totalRounds);
      write(sock,buffer,255);


    }

}
