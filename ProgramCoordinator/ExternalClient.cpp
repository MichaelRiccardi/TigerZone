#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <cstring>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include "ProgramCoordinator.h"
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

char* TOURNAMENT_PASSWD;
char* USERNAME;
char* PASSWD;
bool END_OF_CHALLENGES = 0;
int pid;
int gamePort = 55555;
int x,y;
bool ready = 0;
bool processed = 0;
struct gameMessage* msg = new struct gameMessage;

int createSocket(std::string, int);
void authenticationProtocol(int);
void challengeProtocol(int);
void roundProtocol(int);
void matchProtocol(int);
void moveProtocol(int);
std::string strAtIndex(std::string,int);
void msgThread(int);


int main(int argc, char *argv[])
{
    TOURNAMENT_PASSWD = argv[3];
    USERNAME = argv[4];
    PASSWD = argv[5];

    std::string hostname (argv[1]);
    int portno = atoi(argv[2]);

//SOCKET GOOD TO GO
int sockfd = createSocket(hostname,portno);
std::cout << sockfd <<std::endl;
authenticationProtocol(sockfd);
challengeProtocol(sockfd);

return 0;
}

int createSocket(std::string hostname, int portno)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname(hostname.c_str());

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));

    return sockfd;
}

//AUTHENTICATION PROTOCOL
void authenticationProtocol(int sockfd)
{
    std::string pid;
    char buffer[256];

    //Prepare Output Stream
    std::stringstream out;

    //Server: THIS IS SPARTA!
    read(sockfd,buffer,255);
    printf("%s\n",buffer);

    //Client: JOIN <tournament password>
    bzero(buffer,256);
    out<<"JOIN ";
    out<<TOURNAMENT_PASSWD;
    std::cout<<out.str()<<std::endl;
    write(sockfd,out.str().c_str(),out.gcount()+1);
    out.str("");

    //Server: HELLO!
    bzero(buffer,256);
    read(sockfd,buffer,255);
    printf("%s\n",buffer);

   //Client: I AM <username> <password>
    bzero(buffer,256);
    out<<"I AM ";
    out<<USERNAME<<" "<<PASSWD;
    std::cout<<out.str()<<std::endl;
    write(sockfd,out.str().c_str(),out.gcount()+1);
    out.str("");

    //Server: WELCOME <pid> PLEASE WAIT FOR THE NEXT CHALLENGE
    bzero(buffer,256);
    read(sockfd,buffer,255);
    printf("%s\n",buffer);
     pid = strAtIndex(std::string(buffer),1);
}

void challengeProtocol(int sockfd)
{
    while(!END_OF_CHALLENGES)
    {
        std::string cid;
        int rounds;
        char buffer[256];

        //Server: NEW CHALLENGE <cid> YOU WILL PLAY <rounds> MATCH
        bzero(buffer,256);
        read(sockfd,buffer,255);
        printf("%s\n",buffer);
        cid = strAtIndex(std::string(buffer),1);
        rounds = stoi(strAtIndex(std::string(buffer),6));

        //roundProtocol
        for(int i = 0; i < rounds; i++)
            roundProtocol(sockfd);

       //Server: END OF CHALLENGES or PLEASE WAIT
        bzero(buffer,256);
        read(sockfd,buffer,255);
        printf("%s\n",buffer);
        if(strAtIndex(std::string(buffer),0).compare("END")==0)
        END_OF_CHALLENGES = 1;
    }

}

void roundProtocol(int sockfd)
{
  int rid,rounds;
  char buffer[256];

  //Server: BEGIN ROUND <rid> OF <rounds>
  bzero(buffer,256);
  read(sockfd,buffer,255);
  printf("%s\n",buffer);
    rid = stoi(strAtIndex(std::string(buffer),2));
    rounds = stoi(strAtIndex(std::string(buffer),4));

  //matchProtocol
  matchProtocol(sockfd);

  //Server: END OF ROUND <rid> OF <rounds>
  bzero(buffer,256);
  read(sockfd,buffer,255);
  printf("%s\n",buffer);

}

void matchProtocol(int sockfd)
{


    //variables
    std::string oppPid, tile;
    int orientation, number_tiles, time_plan;
    char buffer[256];

    //Prepare Output Stream
    std::stringstream out;

    //Server: YOUR OPPONENT IS PLAYER <pid>
    bzero(buffer,256);
    read(sockfd,buffer,255);
    printf("%s\n",buffer);
     oppPid = strAtIndex(buffer,4);

    //Server: STARTING TILE IS <tile> AT <x> <y> <orientation>
    bzero(buffer,256);
    read(sockfd,buffer,255);
    printf("%s\n",buffer);
    tile = strAtIndex(std::string(buffer),3);
    x = stoi(strAtIndex(std::string(buffer),5));
    y = stoi(strAtIndex(std::string(buffer),6));
    orientation = stoi(strAtIndex(std::string(buffer),7));

    //PASS STARTING TILE TO INTERNAL SERVER

    //Server: THE REMAINING <number_tiles> TILES ARE [ <tiles> ]
    bzero(buffer,256);
    read(sockfd,buffer,255);
    printf("%s\n",buffer);
    number_tiles = stoi(strAtIndex(std::string(buffer),2));

    //CREATE TILE STACK OF INTERNAL SERVER
    std::string tileStack;
    out<<tile;
    for(int i = 0; i < number_tiles;i++)
        out<<strAtIndex(std::string(buffer),6+i);
    tileStack = out.str();

    //Create Tile Stack Message
    msg -> data.tile.lengthOfStack = 80;
    strcpy(msg -> data.tile.tileStack, tileStack.c_str());

    //Send Tile Stack



    std::cout<<"TILE STACK: "<<out.str()<<std::endl;
    out.str("");

    //Custom Move
    msg -> data.move.p1 = 3;
    strcpy(msg -> data.move.tile, tile.c_str());
    msg -> data.move.x = 0;
    msg -> data.move.y = 0;
    msg -> data.move.orientation = (unsigned int)orientation;


    //Server: MATCH BEGINS IN <timeplan> SECONDS
    bzero(buffer,256);
    read(sockfd,buffer,255);
    printf("%s\n",buffer);
    x = stoi(strAtIndex(std::string(buffer),3));


//    moveProtocol
    for(int i = 1; i <= number_tiles; i++)
    moveProtocol(sockfd);

    //Server: GAME <gid> OVER PLAYER <pid> <score> PLAYER <pid> <score>
    bzero(buffer,256);
    read(sockfd,buffer,255);
    printf("%s\n",buffer);

    //Server: GAME <gid> OVER PLAYER <pid> <score> PLAYER <pid> <score>
    bzero(buffer,256);
    read(sockfd,buffer,255);
    printf("%s\n",buffer);

}

void moveProtocol(int sockfd)
{
    char buffer[256];
    int timeMove, moveNum;
    int gamesActive = 2;
    std::string tile, gid;

    //Prepare Output and msg
    std::stringstream out;
    struct gameMessage* msg = new struct gameMessage;

    //Server: MAKE YOUR MOVE IN GAME <gid> WITHIN <timemove> SECOND: MOVE <#> PLACE <tile>
    bzero(buffer,256);
    read(sockfd,buffer,255);
    printf("%s\n",buffer);
    gid = strAtIndex(std::string(buffer),5);
    timeMove = stoi(strAtIndex(std::string(buffer),7));
    moveNum = stoi(strAtIndex(std::string(buffer),10));
    tile = strAtIndex(std::string(buffer),12);

    //Create Move Message and Pass to INTERNAL Server
    strcpy(msg -> data.move.tile, tile.c_str());

    //Await response

    //Respond to Tournament Server With Move
    int response = 1;
    bzero(buffer,256);

    switch (response) {
        case 0:
            out<<"GAME "<<gid<<" MOVE "<<moveNum<<" TILE "<< tile <<" UNPLACEABLE PASS";
            break;
        case 1:
            out<<"GAME "<<gid<<" MOVE "<<moveNum<<" PLACE "<< tile <<" AT "<<(int)msg->data.move.x + x<<" "<<(int)msg->data.move.y + y<<" "<<(int)msg->data.move.orientation<<" NONE";
            break;
        case 2:
            out<<"GAME "<<gid<<" MOVE "<<moveNum<<" PLACE "<<tile<<" AT "<<(int)msg->data.move.x + x<<" "<<(int)msg->data.move.y + y<<" "<<(int)msg->data.move.orientation<<" TIGER";
            break;
        case 3:
            out<<"GAME "<<gid<<" MOVE "<<moveNum<<" PLACE "<< tile <<" AT "<<(int)msg->data.move.x + x<<" "<<(int)msg->data.move.y + y<<" "<<(int)msg->data.move.orientation<<" CROCODILE";
            break;
    }

    std::cout<<out.str()<<std::endl;
    write(sockfd,out.str().c_str(),out.gcount()+1);
    out.str("");

    //Recieve Results of Turn
    int currentlyActive = gamesActive;
    for(int i = 0; i < currentlyActive; i++){
      int movePid, x, y, orientation, zone;

      //Server: GAME <gid> MOVE <#> PLAYER <pid> <move>
      bzero(buffer,256);
      read(sockfd,buffer,255);
      printf("%s\n",buffer);
      if(strAtIndex(std::string(buffer),6).compare("FORFEITED") == 0) //Game FORFEITED
      {
          gamesActive--;
          //Let Game Know of Forfeit
      }
      else
      {
        if(gid.compare(strAtIndex(std::string(buffer),1)) != 0);    //Game You Care About
        else
        {
            //Let Game Know of the opponents move (not a Forfeit)


        }

      }
  }

}

std::string strAtIndex(std::string buffer, int index)
{
    std::vector<std::string> strings;
    std::string s;
    std::istringstream in (buffer);
    while(getline(in, s, ' '))
      strings.push_back(s);
     return strings[index];
}
