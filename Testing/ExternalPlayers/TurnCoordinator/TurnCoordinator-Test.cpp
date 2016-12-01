#include "../TigerZoneGame/TigerZoneGame.h"
#include "../ExternalPlayers/TurnCoordinator/TurnCoordinator.h"
#include "../Core/BoardManager/BoardManager.h"
#include <string.h>
#include "gtest/gtest.h"


namespace {

class TurnCoordinatorTests : public ::testing::Test {

 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  TurnCoordinatorTests() {
    // You can do set-up work for each test here.
  }

  virtual ~TurnCoordinatorTests() {
    // You can do clean-up work that doesn't throw exceptions here.
    delete game;

  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
    game = new TigerZoneGame();
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  // Objects declared here can be used by all tests in the test case for Foo.
  TigerZoneGame* game;
};


TEST_F(TurnCoordinatorTests, convertEdgeToZone)
{
    TurnCoordinator trn = TurnCoordinator(1, game);
    int returnValue;
    returnValue = trn.convertEdgeToZone(0);
    ASSERT_EQ(returnValue, 1);

    returnValue = trn.convertEdgeToZone(1);
    ASSERT_EQ(returnValue, 2);

    returnValue = trn.convertEdgeToZone(2);
    ASSERT_EQ(returnValue, 3);

    returnValue = trn.convertEdgeToZone(3);
    ASSERT_EQ(returnValue, 3);

    returnValue = trn.convertEdgeToZone(4);
    ASSERT_EQ(returnValue, 6);

    returnValue = trn.convertEdgeToZone(5);
    ASSERT_EQ(returnValue, 9);

    returnValue = trn.convertEdgeToZone(6);
    ASSERT_EQ(returnValue, 9);

    returnValue = trn.convertEdgeToZone(7);
    ASSERT_EQ(returnValue, 8);

    returnValue = trn.convertEdgeToZone(8);
    ASSERT_EQ(returnValue, 7);

    returnValue = trn.convertEdgeToZone(9);
    ASSERT_EQ(returnValue, 7);

    returnValue = trn.convertEdgeToZone(10);
    ASSERT_EQ(returnValue, 4);

    returnValue = trn.convertEdgeToZone(11);
    ASSERT_EQ(returnValue, 1);

    returnValue = trn.convertEdgeToZone(12);
    ASSERT_EQ(returnValue, 5);
}

//Move& trn.convertInMove(gameMessage *msg)
/*
struct moveMessage
{
    unsigned int p1;            //Player flag
    char tile[6];       //Tile Identifier
    bool placeable;     //Can you use tile?
    unsigned int x;              //X coordinate
    unsigned int y;              //Y coordinate
    unsigned int orientation;    //Orientation using network protocol offsets
    int meepleType;     //0: NONE    1: TIGER    2: CROC
    bool pickupMeeple;
    bool pass;
    int zone;           //Zone for meeple if TIGER
};
struct gameMessage
{
    int messageType;
    messageData data;
};


Tile& getTile() const;
const Coord& getCoord() const;
unsigned int getRotation() const;
int getMeepleLocation() const;
bool getHasCrocodile() const;
bool getPickupMeeple() const;
*/

TEST_F(TurnCoordinatorTests, convertInMove)
{
    TurnCoordinator trn = TurnCoordinator(1, game);
    gameMessage *msg = new gameMessage;
    msg->messageType = 1;
    msg->data.move.p1 = 1;
    //printf("What\n");
    char ID[6] = "TJJT-";
    std::copy(ID, ID + 6, msg->data.move.tile);
    //printf("Is\n");
    msg->data.move.placeable = true;
    //printf("Love\n");
    msg->data.move.x = 1;
    //printf("Baby\n");
    msg->data.move.y = 1;
    //printf("Don't\n");
    msg->data.move.orientation = 0;
    //printf("Hurt\n");
    msg->data.move.meepleType = 1;
    //printf("Me\n");
    msg->data.move.pickupMeeple = false;
    //printf("No\n");
    msg->data.move.pass = false;
    //printf("More\n");
    msg->data.move.zone = 1;
    //printf("<3\n");

    Move mv = trn.convertInMove(msg);    
    //printf("Where\n");
    ASSERT_EQ((unsigned int)mv.getCoord().getX(), msg->data.move.x);    
    //printf("Am\n");
    ASSERT_EQ((unsigned int)mv.getCoord().getY(), msg->data.move.y);      
    //printf("I\n");
    ASSERT_EQ(mv.getMeepleLocation(), 0);               //Output for converting zone to edges    
    //printf("Even\n");
    ASSERT_EQ(mv.getRotation(), msg->data.move.orientation);     
    //printf("Going\n");
    ASSERT_FALSE(mv.getHasCrocodile()); 
}

/*
gameMessage trn.buildResponse(Move& move)
{
    gameMessage *gMsg = new gameMessage;
    gMsg->messageType = 1;
    strcpy(gMsg->data.move.tile, (move.getTile().getTileName()).c_str());

    gMsg->data.move.p1 = trn.ourPlayerNumber;

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
        gMsg->data.move.zone = trn.convertEdgeToZone(move.getMeepleLocation());
    }
    else
    {
        gMsg->data.move.meepleType = 0; //None type
    }


    return *gMsg;
}
*/


TEST_F(TurnCoordinatorTests, buildResponse)
{
    TurnCoordinator trn = TurnCoordinator(1, game);
    Move *mv = new Move(game->boardManager->getTopTileStack(), 1, 2, 0, (unsigned int)4);

    gameMessage *gMsg = new gameMessage;
    trn.buildResponse(*mv, gMsg);
    ASSERT_EQ(gMsg->messageType, 1);
    ASSERT_EQ(gMsg->data.move.x, 1);
    ASSERT_EQ(gMsg->data.move.y, 2);
    ASSERT_EQ(gMsg->data.move.orientation, 0);
    ASSERT_EQ(gMsg->data.move.zone, trn.convertEdgeToZone(4)); //Have to account for changing from edge 4 -> zone 6
}

};