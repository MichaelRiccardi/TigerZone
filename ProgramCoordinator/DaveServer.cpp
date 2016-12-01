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

   write(sock,"THIS IS SPARTA!\r\n",17);
   printf("sent message\n");

   bzero(buffer,256);
   read(sock,buffer,255);
   printf("recv'd message\n");
   write(sock,"HELLO!\n",7);
   printf("sent message\n");

   bzero(buffer,256);
   read(sock,buffer,255);
   printf("recv'd message\n");
   sprintf(buffer,"WELCOME %s PLEASE WAIT FOR THE NEXT CHALLENGE\r\n",pid);
   write(sock,buffer,255);
   printf("sent message\n");

//CHALLENGE PROTOCOL
   int totalRounds = 1;

   bzero(buffer,256);
   sprintf(buffer,"NEW CHALLENGE 123 YOU WILL PLAY %d MATCHES\r\n", totalRounds);
   write(sock,buffer,255);

   for(int round = 1; round <= totalRounds; round++) //ROUND PROTOCOL
    {
      bzero(buffer,256);
      sprintf(buffer,"BEGIN ROUND %d of %d\r\n",round ,totalRounds);
      write(sock,buffer,255);

      bzero(buffer,256);
      sprintf(buffer,"YOUR OPPONENT IS PLAYER 24601\r\n");
      write(sock,buffer,255);

      bzero(buffer,256);
      sprintf(buffer,"STARTING TILE IS TLTJ- AT 0 0 0\r\n");
      write(sock,buffer,255);

      bzero(buffer,256);
      sprintf(buffer,"THE REMAINING 6 TILES ARE [ TLTTP LJTJ- JLJL- JJTJX JLTTB TLLT- ]\r\n" );
      write(sock,buffer,255);

      bzero(buffer,256);
      sprintf(buffer,"MATCH BEGINS IN 5 SECONDS\r\n");
      write(sock,buffer,255);
      for(int i = 1; i <= 5; i++){

          bzero(buffer,256);
          sprintf(buffer,"MAKE YOUR MOVE IN GAME A WITHIN 1 SECOND: MOVE %d PLACE JLJL-\r\n", i);
          write(sock,buffer,255);

          bzero(buffer,256);
          read(sock,buffer,255);

          bzero(buffer,256);
          sprintf(buffer,"GAME A MOVE %i PLAYER 24601 PLACED LJTJ- AT 2 2 270 NONE\r\n", i);
          write(sock,buffer,255);

          bzero(buffer,256);
          sprintf(buffer,"GAME B MOVE %i PLAYER 12345 PLACED LJTJ- AT 2 25 90 NONE\r\n", i);
          write(sock,buffer,255);

      }

      bzero(buffer,256);
      sprintf(buffer,"GAME A OVER PLAYER <pid> <score> PLAYER <pid> <score>\r\n");
      write(sock,buffer,255);

      bzero(buffer,256);
      sprintf(buffer,"GAME B OVER PLAYER <pid> <score> PLAYER <pid> <score>\r\n");
      write(sock,buffer,255);

      bzero(buffer,256);
      sprintf(buffer,"END OF ROUND %d OF %d\r\n", round, totalRounds);
      write(sock,buffer,255);
    }
    bzero(buffer,256);
    sprintf(buffer,"END OF CHALLENGES\r\n");
    write(sock,buffer,255);

}
