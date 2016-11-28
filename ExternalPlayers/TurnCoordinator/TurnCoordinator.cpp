#include "TurnCoordinator.h"


unsigned int TurnCoordinator::ourPlayerNumber = 0;
unsigned int TurnCoordinator::otherPlayerNumber = 0;
bool TurnCoordinator::AISetup = false;
int TurnCoordinator::mySocket = 0;
struct sockaddr_in *TurnCoordinator::myAddr = NULL;
struct sockaddr_in *TurnCoordinator::clientAddr = NULL;

//Main functionality is to receive messages from the external game client and tell the AI when to take a turn.
//It will also call the BoardManager to update opponent moves.

TurnCoordinator::TurnCoordinator(int port)
{
    TurnCoordinator::AISetup = false;
    TurnCoordinator::ourPlayerNumber = 0;
    TurnCoordinator::otherPlayerNumber = 0;

    TurnCoordinator::myAddr = new struct sockaddr_in;

    setupSocket(port);
}

TurnCoordinator::~TurnCoordinator()
{
    close(TurnCoordinator::mySocket);
}

void TurnCoordinator::setupSocket(std::string hostname, int portNumber)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    TurnCoordinator::myAddr = &serv_addr;
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

    TurnCoordinator::mySocket = sockfd;
}

void TurnCoordinator::setUpAI()
{
    if(TurnCoordinator::ourPlayerNumber == 0)
    {
        throw std::logic_error("Trying to setup the AI before you know which player it is.");
    }
    AI::setPlayerNumber(TurnCoordinator::ourPlayerNumber);
    TurnCoordinator::AISetup = true;
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


gameMessage TurnCoordinator::buildResponse(Move& move)
{
    gameMessage *gMsg = new gameMessage;
    gMsg->messageType = 1;
    strcpy(gMsg->data.move.tile, (move.getTile().getTileName()).c_str());

    gMsg->data.move.p1 = TurnCoordinator::ourPlayerNumber;

    gMsg->data.move.x = move.getCoord().getX();
    gMsg->data.move.y = move.getCoord().getY();
    gMsg->data.move.orientation = move.getTile().getRotation();

    /*if(move.getHasCrocodile())
    {
        gMsg->data.move.meepleType = 2; //Croc Type
    }
    else if(move.getMeepleLocation() != -1)
    {
        gMsg->data.move.meepleType = 1; //Meeple Type
        gMsg->data.move.zone = TurnCoordinator::convertEdgeToZone(move.getMeepleLocation());
    }*/
    //else
    //{
        gMsg->data.move.meepleType = 0; //None type
    //}


    return *gMsg;
}


void TurnCoordinator::callAI()
{
    if(!TurnCoordinator::AISetup)
    {
        if(TurnCoordinator::ourPlayerNumber == 0)
        {
            throw std::logic_error("Trying to call the AI before you know which player it is.");
        }
        TurnCoordinator::setUpAI();
    }
    Move chosenMove = AI::chooseTurn(BoardManager::getTopTileStack());
    BoardManager::makeMove(chosenMove, TurnCoordinator::ourPlayerNumber);

    gameMessage msg = TurnCoordinator::buildResponse(chosenMove);

    int n = write(TurnCoordinator::clientSocket, (char *)(&msg), sizeof(msg));

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
        mv = new Move((Tile&)BoardManager::getTopTileStack(), msg->data.move.pickupMeeple);
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
    //std::cout << "TopTile : " << BoardManager::getTopTileStack().getTileName() << "MsgTile : " << msg->data.move.tile << std::endl;
    if(!BoardManager::getTopTileStack().getTileName().compare(msg->data.move.tile))
    {
        throw std::logic_error("Top of the tile stack and current tile move do not match");
    }
    //printf("It's\n");
    switch(msg->data.move.meepleType)
    {
        case 0:
            //No meeple
            mv = new Move((Tile&)BoardManager::getTopTileStack(), msg->data.move.x, msg->data.move.y, msg->data.move.orientation);
            break;
        case 1:
            //Tiger
            mv = new Move((Tile&)BoardManager::getTopTileStack(), msg->data.move.x, msg->data.move.y, msg->data.move.orientation, zone);
            break;
        case 2:
            //Croc
            mv = new Move((Tile&)BoardManager::getTopTileStack(), msg->data.move.x, msg->data.move.y, msg->data.move.orientation, true);
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
            BoardManager::inputTileStack(msg->data.tile.tileStack, msg->data.tile.lengthOfStack);
            break;
        case 1:
            if(msg->data.move.p1 != TurnCoordinator::ourPlayerNumber)
            {
                Move *mv = &(TurnCoordinator::convertInMove(msg));
                if(!(msg->data.move.placeable) && !(msg->data.move.pass))
                {
                    BoardManager::cannotPlaceTile(*mv, TurnCoordinator::otherPlayerNumber);
                }
                else if(!(msg->data.move.pass))
                {
                    BoardManager::makeMove(*mv, TurnCoordinator::otherPlayerNumber);
                }
                else
                {
                    break;
                }
            }
            else if(msg->data.move.p1 == 3)
            {
                Move *mv = &(TurnCoordinator::convertInMove(msg));
                BoardManager::makeMove(*mv, TurnCoordinator::otherPlayerNumber);
            }
            else
            {
                if(!strcmp((BoardManager::getTopTileStack()).getTileName().c_str(), msg->data.move.tile))
                {
                    throw std::logic_error("Top of the tile stack and current tile move do not match");
                }
                TurnCoordinator::callAI();
            }
            break;
        case 2:
            if(msg->data.who.p1 == 1)
            {
                TurnCoordinator::ourPlayerNumber = 1;
                TurnCoordinator::otherPlayerNumber = 2;
            }
            else
            {
                TurnCoordinator::otherPlayerNumber = 1;
                TurnCoordinator::ourPlayerNumber = 2;
            }
            TurnCoordinator::setUpAI();
            break;
        default:
            break;
    }
}

void TurnCoordinator::startCoordinator()
{

    int clilen;
    struct sockaddr_in cli_addr;

    //Wait for a connection
    listen(TurnCoordinator::mySocket,5);
    clilen = sizeof(cli_addr);

    //Accept the new connection
    TurnCoordinator::clientSocket = accept(TurnCoordinator::mySocket, (struct sockaddr *) &cli_addr,(socklen_t *) &clilen);

    //Hanlde any game init stuff.

    TurnCoordinator::receiveMessage();
}


void TurnCoordinator::receiveMessage()
{
    int n;

    char buffer[sizeof(gameMessage)]; //Change to message struct.
    gameMessage *msg = (gameMessage *)(&buffer);

    while(true)
    {
        if (TurnCoordinator::mySocket < 0)
        {
            throw std::runtime_error("ERROR on accept");
        }

        //Clear out any previous data
        bzero(buffer, sizeof(gameMessage));
        n = read(TurnCoordinator::mySocket, buffer, sizeof(gameMessage) - 1);

        if (n < 0)
        {
            throw std::runtime_error("ERROR reading from socket");
        }

        //Handle message here
        TurnCoordinator::handleMessage(msg);
    }
}
