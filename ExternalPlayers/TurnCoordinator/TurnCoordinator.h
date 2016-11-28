#define WHY_ME

#ifndef __TURN_COORDINATOR_H
#define __TURN_COORDINATOR_H
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
    static void handleMessage(gameMessage *msg);
    static gameMessage getMessage();

#ifdef __testing
#else
private:
#endif
    static void callAI();
    static void setUpAI();
    static void setupSocket(int portNumber);
    static int convertEdgeToZone(int edge);
    static void buildResponse(Move& move);
    static Move& convertInMove(gameMessage *msg);
    static unsigned int ourPlayerNumber;
    static unsigned int otherPlayerNumber;
    static bool AISetup;
    static gameMessage myMessage;
};


#endif
