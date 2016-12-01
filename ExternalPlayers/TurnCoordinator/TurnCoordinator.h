#define WHY_ME

#ifndef __TURN_COORDINATOR_H
#define __TURN_COORDINATOR_H
#include "../../TigerZoneGame/TigerZoneGame.h"
#include "../../ProgramCoordinator/ProgramCoordinator.h"
#include "../AI/AI.h"

#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <iostream>

#include <unistd.h>

//Needs structure(s) defined for the messages from the external game client.


//Define message types

class TurnCoordinator
{
public:
    TurnCoordinator(int port, TigerZoneGame* game);
    ~TurnCoordinator();
    void startCoordinator();  //Infinite loop for receiving messages from external client
#ifdef __testing
#else
private:
#endif
    TigerZoneGame* game;
    void receiveMessage();
    void handleMessage(gameMessage *msg);
    void callAI();
    void setUpAI();
    void setupSocket(int portNumber);
    int convertEdgeToZone(int edge);
    void buildResponse(Move& move, gameMessage *gMsg);
    Move& convertInMove(gameMessage *msg);
    unsigned int ourPlayerNumber;
    unsigned int otherPlayerNumber;
    bool AISetup;
    int mySocket;
    int clientSocket;
    struct sockaddr_in *myAddr;
    struct sockaddr_in *clientAddr;
};


#endif
