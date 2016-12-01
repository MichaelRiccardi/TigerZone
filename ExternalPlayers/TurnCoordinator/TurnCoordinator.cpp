#include "TurnCoordinator.h"
//Main functionality is to receive messages from the external game client and tell the AI when to take a turn.
//It will also call the BoardManager to update opponent moves.

TurnCoordinator::TurnCoordinator(int port, TigerZoneGame* game)
{
    this->game = game;
    this->AISetup = false;
    this->ourPlayerNumber = 0;
    this->otherPlayerNumber = 0;
    this->myAddr = new struct sockaddr_in;
    this->clientAddr = new struct sockaddr_in;
#ifndef __testing
    setupSocket(port);
#endif
}

TurnCoordinator::~TurnCoordinator()
{
    close(this->clientSocket);
    close(this->mySocket);
}

void TurnCoordinator::setupSocket(int portNumber)
{
    /* First call to socket() function */
    struct hostent *server;
    this->mySocket = socket(AF_INET, SOCK_STREAM, 0);
   
    if (this->mySocket < 0) 
    {
        throw std::runtime_error("ERROR opening socket");
    }
    server = gethostbyname("localhost");
    if (server == NULL)
    {
        throw std::runtime_error("ERROR can't gethostbyname()");
    }
   
    /* Initialize socket structure */
    bzero((char *) this->myAddr, sizeof(*this->myAddr));
   
    this->myAddr->sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&this->clientAddr->sin_addr.s_addr, server->h_length);
    this->myAddr->sin_port = htons(portNumber);
   
   /* Now bind the host address using bind() call.*/
    if (connect(this->mySocket, (struct sockaddr *) this->clientAddr, sizeof(this->clientAddr)) < 0) 
    {
        throw std::runtime_error("ERROR connecting socket");
    }
}

void TurnCoordinator::setUpAI()
{
    if(this->ourPlayerNumber == 0)
    {
        throw std::logic_error("Trying to setup the AI before you know which player it is.");
    }
    game->ai->setPlayerNumber(this->ourPlayerNumber);
    this->AISetup = true;
}

int TurnCoordinator::convertEdgeToZone(int edge)
{
    switch (edge)
    {
        case 0:
            return 1;
            break;
        case 1:
            return 2;
            break;
        case 2:
        case 3:
            return 3;
            break;
        case 4:
            return 6;
            break;
        case 5:
        case 6:
            return 9;
            break;
        case 7:
            return 8;
            break;
        case 8:
        case 9:
            return 7;
            break;
        case 10:
            return 4;
            break;
        case 11:
            return 1;
            break;
        case 12:
            return 5;
            break;
        default:
            throw std::logic_error("Invalid edge input for convertEdgeToZone");
            return 0;
            break;
    }
}


void TurnCoordinator::buildResponse(Move& move, gameMessage *gMsg)
{
    bzero(gMsg, sizeof(*gMsg));
    gMsg->messageType = 1;
    strcpy(gMsg->data.move.tile, (move.getTile().getTileName()).c_str());

    gMsg->data.move.p1 = ourPlayerNumber;

    gMsg->data.move.x = move.getCoord().getX();
    gMsg->data.move.y = move.getCoord().getY();
    gMsg->data.move.orientation = move.getTile().getRotation();
    
    if(move.getHasCrocodile())
    {
        gMsg->data.move.meepleType = 2; //Croc Type
    }
    else if(move.getMeepleLocation() != -1)
    {
        gMsg->data.move.meepleType = 1; //Meeple Type
        gMsg->data.move.zone = convertEdgeToZone(move.getMeepleLocation());
    }
    else
    {
        gMsg->data.move.meepleType = 0; //None type
    }

}


void TurnCoordinator::callAI()
{
    if(!this->AISetup)
    {
        if(this->ourPlayerNumber == 0)
        {
            throw std::logic_error("Trying to call the AI before you know which player it is.");
        }
        setUpAI();
    }
    Move chosenMove = game->ai->chooseTurn(game->boardManager->getTopTileStack());
    game->boardManager->makeMove(chosenMove, this->ourPlayerNumber);

    gameMessage *msg = new gameMessage;
    buildResponse(chosenMove, msg);

    int n = write(this->clientSocket, (char *)(msg), sizeof(*msg));

    if (n < 0) 
    {
        throw std::runtime_error("ERROR writing to socket");
    }
}

Move& TurnCoordinator::convertInMove(gameMessage *msg)
{
    Move *mv;
    //printf("Who's\n");
    if(!(msg->data.move.placeable) && !(msg->data.move.pass))
    {
        mv = new Move((Tile&)game->boardManager->getTopTileStack(), msg->data.move.pickupMeeple);
        return (*mv);
    }
    unsigned int zone;
    //printf("That\n");
    switch (msg->data.move.zone)
    {
        case 1:
            zone = 0;
            break;
        case 2:
            zone = 1;
            break;
        case 3:
            zone = 2;
            break;
        case 4:
            zone = 10;
            break;
        case 5:
            zone = 12;
            break;
        case 6:
            zone = 4;
            break;
        case 7:
            zone = 8;
            break;
        case 8:
            zone = 7;
            break;
        case 9:
            zone = 5;
            break;
        default:
            throw std::logic_error("Zone not recognized");
            break;
    }
    //printf("Girl\n");
    //std::cout << "TopTile : " << game->boardManager->getTopTileStack().getTileName() << "MsgTile : " << msg->data.move.tile << std::endl;
    if(!game->boardManager->getTopTileStack().getTileName().compare(msg->data.move.tile))
    {
        throw std::logic_error("Top of the tile stack and current tile move do not match");
    }
    //printf("It's\n");
    switch(msg->data.move.meepleType)
    {
        case 0:
            //No meeple
            mv = new Move((Tile&)game->boardManager->getTopTileStack(), msg->data.move.x, msg->data.move.y, msg->data.move.orientation);
            break;
        case 1:
            //Tiger
            mv = new Move((Tile&)game->boardManager->getTopTileStack(), msg->data.move.x, msg->data.move.y, msg->data.move.orientation, zone);
            break;
        case 2:
            //Croc
            mv = new Move((Tile&)game->boardManager->getTopTileStack(), msg->data.move.x, msg->data.move.y, msg->data.move.orientation, true);
            break;
        default:
            throw std::logic_error("Unrecognized meeple type");
            break;
    }
    //printf("Jess\n");
    return (*mv);
}

void TurnCoordinator::handleMessage(gameMessage *msg)
{
    //Determine if we need to pick a move
    switch(msg->messageType)
    {
        case 0:
            game->boardManager->inputTileStack(msg->data.tile.tileStack, msg->data.tile.lengthOfStack);
            break;
        case 1:
            if(msg->data.move.p1 != this->ourPlayerNumber)
            {
                Move mv = convertInMove(msg);
                if(!(msg->data.move.placeable) && !(msg->data.move.pass))
                {
                    game->boardManager->cannotPlaceTile(mv, this->otherPlayerNumber);
                }
                else if(!(msg->data.move.pass))
                {
                    game->boardManager->makeMove(mv, this->otherPlayerNumber);
                }
                else
                {
                    break;
                }
            }
            else if(msg->data.move.p1 == 3)
            {
                Move mv = convertInMove(msg);
                game->boardManager->makeMove(mv, this->otherPlayerNumber);
            }
            else
            {
                if(!strcmp((game->boardManager->getTopTileStack()).getTileName().c_str(), msg->data.move.tile)) 
                {
                    throw std::logic_error("Top of the tile stack and current tile move do not match");
                }
                callAI();
            }
            break;
        case 2:
            if(msg->data.who.p1 == 1)
            {
                this->ourPlayerNumber = 1;
                this->otherPlayerNumber = 2;
            }
            else
            {
                this->otherPlayerNumber = 1;
                this->ourPlayerNumber = 2;                
            }
            setUpAI();
            break;
        default:
            break;
    }
}




void TurnCoordinator::startCoordinator()
{
    receiveMessage();
}


void TurnCoordinator::receiveMessage()
{
    int n;

    char buffer[sizeof(gameMessage)]; //Change to message struct.
    gameMessage *msg = (gameMessage *)(&buffer);

    while(true)
    {
        if (this->clientSocket < 0) 
        {
            throw std::runtime_error("ERROR on accept");
        }

        //Clear out any previous data
        bzero(buffer, sizeof(gameMessage));
        n = read(this->clientSocket, buffer, sizeof(gameMessage) - 1);

        if (n < 0) 
        {
            throw std::runtime_error("ERROR reading from socket");
        }

        //Handle message here
        handleMessage(msg);
    }
}
