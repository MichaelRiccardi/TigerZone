#include "../Core/BoardRules/Regions.h"
#include "../Core/Board/Board.h"
#include "../Core/BoardManager/BoardManager.h"
#include "../Core/BoardRules/Rules.h"
#include "../TigerZoneGame/TigerZoneGame.h"

#include "gtest/gtest.h"

namespace {

class BoardRulesTests : public ::testing::Test {

 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  BoardRulesTests() {
    // You can do set-up work for each test here.
  }

  virtual ~BoardRulesTests() {
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

    void testingTilePlacement(unsigned int *startID, unsigned int coordX, unsigned int coordY, Tile* currentTile, const Tile **surroundingTiles)
    {
        Coord *currentCoord = new Coord(coordX, coordY); //Center
        Move *currentMove = new Move(*currentTile, *currentCoord);
        game->board->place(*currentMove);
        surroundingTiles = game->board->getBorderingTiles(*currentTile);
        game->regions->addConnection(*currentTile, surroundingTiles);
    }

  // Objects declared here can be used by all tests in the test case for Foo.
  TigerZoneGame* game;
};

TEST_F(BoardRulesTests, mergeRegions) 
{
    std::shared_ptr<struct regionSet> r1 = std::shared_ptr<struct regionSet>(new regionSet);
    std::shared_ptr<struct regionSet> r2 = std::shared_ptr<struct regionSet>(new regionSet);

    r1->edgesTillCompletion = 4;
    std::shared_ptr<struct tileNode> currentNode = std::shared_ptr<struct tileNode>(new tileNode);
    r1->head = currentNode;

    std::shared_ptr<struct regionSet > *regionArray = new std::shared_ptr< struct regionSet>[13];
    for(int i = 0; i < 13; i++)
    {
        ////printf("TileNode %d at %X\n", i, currentNode.get());
        currentNode->tileID = i;
        currentNode->edge = i;
        if (i + 1 < 13)
        {
            currentNode->next = std::shared_ptr<struct tileNode>(new tileNode);
            currentNode = currentNode->next;
        }

        game->regions->regionTracker[i] = regionArray;
        regionArray[i] = r1;
    }
    r1->tail = currentNode;

    r2->edgesTillCompletion = 2;
    r2->head = std::shared_ptr<struct tileNode>(new tileNode);
    currentNode = r2->head;
    regionArray = new std::shared_ptr< struct regionSet>[13];
    for(int i = 0; i < 13; i++)
    {
        ////printf("TileNode %d at %X\n", i + 13, currentNode.get());
        currentNode->tileID = i + 13;
        currentNode->edge = i;
        if (i + 1 < 13)
        {
            currentNode->next = std::shared_ptr<struct tileNode>(new tileNode);
            currentNode = currentNode->next;
        }

        game->regions->regionTracker[i + 13] = regionArray;   
        regionArray[i] = r2;
    }
    r2->tail = currentNode;

    unsigned int preMergeTotalEdges = r1->edgesTillCompletion + r2->edgesTillCompletion;
    ////printf("r1 : %X r2 : %x\n", r1.get(), r2.get());
    game->regions->mergeRegions(0, 0, 13, 0);

    EXPECT_EQ(game->regions->regionTracker[0][0], r2);
    EXPECT_EQ(game->regions->regionTracker[13][0], r2);    
    EXPECT_NE(r1->head.get(), r2->head.get());
    EXPECT_EQ(r1->tail.get(), r2->tail.get());
    EXPECT_EQ(r2->edgesTillCompletion, preMergeTotalEdges);

}

TEST_F(BoardRulesTests, addConnection) {

    game->boardManager->gameInit();
    unsigned int startID = 18000;
    //Tile currentTile;
    //Move *currentMove;
    //Coord *currentCoord;
    const Tile **surroundingTiles = NULL; //Init'd to NULL

    unsigned int centerID = game->board->get(Coord(76,76))->getId();
    /*
    //printf("centerID : %d\n", centerID);

    for(int i = 0; i < 12; i++)
    {
        //printf("edge : %d edgesTillCompletion : %d\n", i, game->regions->regionTracker[centerID][i]->edgesTillCompletion);
    }
    */


    EXPECT_EQ((game->regions->checkRegionEdgesTillCompletion(centerID, 1)), 2); //Make sure the road has two sides open.

    Tile tileW = (Tile::CreateTileW(1, startID, PreyType::None)[0]); //No prey, starting tile
    Tile * currentTile = &tileW;
    //testingTilePlacement(&startID, 72, 73, currentTile, surroundingTiles);

    game->boardManager->makeMove(Move(*currentTile, Coord(76, 75)), 1);

    for(int i = 0; i < 12; i++)
    {
        //Make sure everything has a region
        EXPECT_TRUE(game->regions->checkRegionExistence(currentTile->getId(),i));
    }

    EXPECT_EQ((game->regions->checkRegionEdgesTillCompletion(currentTile->getId(), 1)), 1); //Make sure the top road has one side open.
    EXPECT_EQ((game->regions->checkRegionEdgesTillCompletion(currentTile->getId(), 7)), 1); //Make sure the right road has one side open.
    EXPECT_EQ((game->regions->checkRegionEdgesTillCompletion(currentTile->getId(), 10)), 1); //Make sure the left road has one side open.

    Tile tileT = (Tile::CreateTileT(1, startID, PreyType::None)[0]); //No prey, starting tile
    currentTile = &tileT;
    //testingTilePlacement(&startID, 72, 73, currentTile, surroundingTiles);

    game->boardManager->makeMove(Move(*currentTile, Coord(75, 75), 1), 1);

    for(int i = 0; i < 12; i++)
    {
        //Make sure everything has a region
         EXPECT_TRUE(game->regions->checkRegionExistence(currentTile->getId(),i));
    }

    EXPECT_EQ((game->regions->checkRegionEdgesTillCompletion(currentTile->getId(), 4)), 0); //Make sure the road has zero sides open.
    EXPECT_EQ((game->regions->checkRegionEdgesTillCompletion(currentTile->getId(), 10)), 3); //Make sure the castle has three sides open.

    Tile tileH = (Tile::CreateTileH(1, startID, PreyType::None)[0]); //No prey, starting tile
    currentTile = &tileH;
    //testingTilePlacement(&startID, 73, 72, currentTile, surroundingTiles);
    game->boardManager->makeMove(Move(*currentTile, Coord(77, 75)), 1);
    //std::cout << currentTile->getTileName() << std::endl;


    for(int i = 0; i < 12; i++)
    {
        //Make sure everything has a region
        EXPECT_TRUE(game->regions->checkRegionExistence(currentTile->getId(),i));

    }

    EXPECT_EQ((game->regions->checkRegionEdgesTillCompletion(currentTile->getId(), 1)),1); //Make sure the right castle has one side open.
    EXPECT_EQ((game->regions->checkRegionEdgesTillCompletion(currentTile->getId(), 7)),1); //Make sure the left castle has zero sides open.

    Tile tileV = (Tile::CreateTileV(1, startID, PreyType::None)[0]); //No prey, starting tile
    currentTile = &tileV;
    //testingTilePlacement(&startID, 73, 73, currentTile, surroundingTiles);
    game->boardManager->makeMove(Move(*currentTile, Coord(76, 74), 1), 1);

    for(int i = 0; i < 12; i++)
    {
        //Make sure everything has a region
        EXPECT_TRUE(game->regions->checkRegionExistence(currentTile->getId(),i));
    }
    
    //std::cout << currentTile->getTileName() << std::endl;

    EXPECT_EQ((game->regions->checkRegionEdgesTillCompletion(currentTile->getId(), 4)), 1); //Make sure the road has one side open.

    Tile tileA = (Tile::CreateTileA(1, startID, PreyType::None)[0]); //No prey, starting tile
    currentTile = &tileA;
    //testingTilePlacement(&startID, 71, 73, currentTile, surroundingTiles);
    game->boardManager->makeMove(Move(*currentTile, Coord(76, 77)), 1);

    for(int i = 0; i < 12; i++)
    {
        //Make sure everything has a region
        EXPECT_TRUE(game->regions->checkRegionExistence(currentTile->getId(),i));
    }

    EXPECT_EQ((game->regions->checkRegionEdgesTillCompletion(currentTile->getId(), 7)),0); //Make sure the road has one side open.
    EXPECT_EQ((game->regions->checkRegionEdgesTillCompletion(currentTile->getId(), 12)),7); //Make sure the church has fives sides open.

    Tile tileB = (Tile::CreateTileB(1, startID, PreyType::None)[0]); //No prey, starting tile
    currentTile = &tileB;
    //testingTilePlacement(&startID, 71, 72, currentTile, surroundingTiles);
    game->boardManager->makeMove(Move(*currentTile, Coord(77, 77)), 1);

    for(int i = 0; i < 12; i++)
    {
        //Make sure everything has a region
        EXPECT_TRUE(game->regions->checkRegionExistence(currentTile->getId(),i));
    }
    EXPECT_EQ((game->regions->checkRegionEdgesTillCompletion(currentTile->getId(), 12)),6); //Make sure the church has three sides open.

    ////printf("Seg fault yet?\n");
    Tile tileE = (Tile::CreateTileE(1, startID, PreyType::None)[0]); //No prey, starting tile
    ////printf("Nah\n");
    currentTile = &tileE;
    //testingTilePlacement(&startID, 71, 71, currentTile, surroundingTiles);
    game->boardManager->makeMove(Move(*currentTile, Coord(75, 76), 2), 1);
    ////printf("Moves suck\n");

    for(int i = 0; i < 12; i++)
    {
        //Make sure everything has a region
        EXPECT_TRUE(game->regions->checkRegionExistence(currentTile->getId(),i));
    }

    ////printf("For loop is happy\n");
    EXPECT_EQ((game->regions->checkRegionEdgesTillCompletion(currentTile->getId(), 7)),2); //Make sure the castle has three sides open.
    ////printf("Keep going\n");

    Tile tileN = (Tile::CreateTileN(1, startID, PreyType::None)[0]); //No prey, starting tile

    ////printf("N is for no\n");
    currentTile = &tileN;
    //testingTilePlacement(&startID, 73, 71, currentTile, surroundingTiles);
    game->boardManager->makeMove(Move(*currentTile, Coord(77, 76), 2), 1);
    ////printf("Make that move\n");

    for(int i = 0; i < 12; i++)
    {
        //Make sure everything has a region
        EXPECT_TRUE(game->regions->checkRegionExistence(currentTile->getId(),i));

    }

    EXPECT_EQ((game->regions->checkRegionEdgesTillCompletion(currentTile->getId(), 10)),0); //Make sure the castle has three sides open.

    Tile tileL = (Tile::CreateTileL(1, startID, PreyType::None)[0]); //No prey, starting tile
    currentTile = &tileL;
    //testingTilePlacement(&startID, 73, 71, currentTile, surroundingTiles);
    game->boardManager->makeMove(Move(*currentTile, Coord(77, 74), 3), 2);

    for(int i = 0; i < 12; i++)
    {
        //Make sure everything has a region
        EXPECT_TRUE(game->regions->checkRegionExistence(currentTile->getId(),i));

    }

    EXPECT_EQ((game->regions->checkRegionEdgesTillCompletion(currentTile->getId(), 10)),0); //Make sure the road is completed
    EXPECT_EQ((game->regions->checkRegionEdgesTillCompletion(currentTile->getId(), 1)),0); //Make sure the castle is completed
}

TEST_F(BoardRulesTests, ScoreChurch) {
    unsigned int startID = 18000;
    Tile *currentTile;
    //Move *currentMove;
    //Coord *currentCoord;
    const Tile **surroundingTiles;
    game->boardManager->gameInit();

    unsigned int expectedScore = 1;
    currentTile = &(Tile::CreateTileB(1, startID, PreyType::None)[0]);

    unsigned int currentTileID = currentTile->getId();
    unsigned int centerChurchID = currentTileID;
    currentTile->setRotation(0);
    game->boardManager->makeMove(Move(*currentTile, Coord(72, 72), 0), 2);
    currentTile->placeTile();

    Coord *churchCoord = new Coord(72, 72);
    unsigned int tilesSurrounded = game->boardManager->isSurrounded(currentTileID);

    ////printf("Found %d tiles\n", tilesSurrounded);
    unsigned int actualScore = game->rules->scoreChurch(tilesSurrounded, true);
    EXPECT_EQ(actualScore, 0);
    actualScore = game->rules->scoreChurch(tilesSurrounded, false);
    EXPECT_EQ(actualScore,1);
    ////printf("scored\n");

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue; // avoid overwriting the church tile
            }
            else
            {
                // place field tiles around church tile
                currentTile = &(Tile::CreateTileY(1, startID, PreyType::None)[0]);
                currentTileID = currentTile->getId();
                currentTile->setRotation(0);
                testingTilePlacement(&startID, (churchCoord->getX() + i), (churchCoord->getY() + j), currentTile, surroundingTiles);
                currentTile->placeTile();

                expectedScore++;
                tilesSurrounded = game->boardManager->isSurrounded(centerChurchID);
                ////printf("placed tile %d at %d %d, expected score %d, surrounded %d\n", currentTileID, churchCoord->getX() + i, churchCoord->getY() + j, expectedScore, tilesSurrounded);


                actualScore = game->rules->scoreChurch(tilesSurrounded, true);

                // just placed final boardering tile, actuallyScore set to true should return full value
                if (i == 1 && j == 1)
                {
                    EXPECT_EQ(actualScore,expectedScore);
                }

                // church isn't completely surrounded, actuallyScore set to true should return 0
                else
                {
                    EXPECT_EQ(actualScore,0);
                }
                actualScore = game->rules->scoreChurch(tilesSurrounded, false);
                EXPECT_EQ(actualScore,expectedScore);
            }
        }
    }
}

TEST_F(BoardRulesTests, ScoreCastle1) {
    unsigned int startID = 0;
    game->boardManager->gameInit();
    Tile *currentTile;
    //Move *currentMove;
    //Coord *currentCoord;
    const Tile **surroundingTiles;

    /*currentTile = &(Tile::CreateTileD(1, startID, PreyType::Deer)[0]);
    unsigned int currentTileID = currentTile->getId();
    currentTile->setRotation(0);
    testingTilePlacement(&startID, 72, 72, currentTile, surroundingTiles);
    currentTile->placeTile();

    */
    std::shared_ptr<struct regionSet> currentSet = std::shared_ptr<struct regionSet>(new regionSet);
    std::shared_ptr<struct tileNode> currentTileNode = std::shared_ptr<struct tileNode>(new tileNode);
    currentSet->head = currentTileNode;

    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < NUM_PREY; j++)
        {
            currentTileNode->preyCounts[j] = 0;
        }
        currentTileNode->tileID = i;
        currentTileNode->edge = i;
        if(i+1 < 5)
        {
            currentTileNode->next = std::shared_ptr<struct tileNode>(new tileNode);
            currentTileNode = currentTileNode->next;
        }
    }
    currentSet->tail = currentTileNode;
    currentTileNode->next = NULL;

    currentSet->edgesTillCompletion = 1;



    unsigned int actualScore = game->rules->scoreCastle(currentSet, true, false);
    ////printf("Where am i\n");
    EXPECT_EQ(actualScore,0);
    actualScore = game->rules->scoreCastle(currentSet, false, false);
    EXPECT_EQ(actualScore,10);
    ////printf("Where am i\n");

    // add another tile to extend the lake region
    /*currentTile = &(Tile::CreateTileK(1, startID, PreyType::Boar)[0]);
    currentTileID = currentTile->getId();
    currentTile->setRotation(2);
    testingTilePlacement(&startID, 73, 72, currentTile, surroundingTiles);
    currentTile->placeTile();
    //printf("Where am i\n");
    */
    //currentSet = game->regions->getRegions(currentTileID);

    ////printf("\nADDING NEW TILE NODES\n");

    currentTileNode->next = std::shared_ptr<struct tileNode>(new tileNode);
    currentTileNode = currentTileNode->next;
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < NUM_PREY; j++)
        {
            currentTileNode->preyCounts[j] = 0;
        }
        currentTileNode->preyCounts[0] = 1;
        currentTileNode->tileID = i+5;
        currentTileNode->edge = i;
        if(i+1 < 5)
        {
            currentTileNode->next = std::shared_ptr<struct tileNode>(new tileNode);
            currentTileNode = currentTileNode->next;
        }
    }
    currentSet->tail = currentTileNode;
    currentTileNode->next = NULL;

    currentSet->edgesTillCompletion = 0;

    actualScore = game->rules->scoreCastle(currentSet, true, false);
    EXPECT_EQ(actualScore,30);
    actualScore = game->rules->scoreCastle(currentSet, false, false);
    EXPECT_EQ(actualScore,30);
    ////printf("Where am i\n");

}
/*
TEST_F(BoardRulesTests, ScoreCastle2) {
    unsigned int startID = 0;
    Tile *currentTile;
    //Move *currentMove;
    //Coord *currentCoord;
    const Tile **surroundingTiles;

    currentTile = &(Tile::CreateTileI(1, startID, PreyType::None)[0]);
    currentTile->setRotation(0);
    testingTilePlacement(&startID, 72, 72, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileJ(1, startID, PreyType::Buffalo)[0]);
    currentTile->setRotation(0);
    testingTilePlacement(&startID, 71, 73, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileC(1, startID, PreyType::None)[0]);
    currentTile->setRotation(0);
    testingTilePlacement(&startID, 72, 73, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileE(1, startID, PreyType::None)[0]);
    currentTile->setRotation(0);
    testingTilePlacement(&startID, 72, 74, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileN(1, startID, PreyType::None)[0]);
    currentTile->setRotation(3);
    testingTilePlacement(&startID, 73, 73, currentTile, surroundingTiles);
    currentTile->placeTile();

    unsigned int currentTileID = currentTile->getId();
    std::shared_ptr<struct regionSet> *currentSet = game->regions->getRegions(currentTileID);

    unsigned int actualScore = game->rules->scoreCastle(*currentSet, true, false);
    EXPECT_EQ(actualScore,0);
    actualScore = game->rules->scoreCastle(*currentSet, false, false);
    EXPECT_EQ(actualScore,10);
    
    currentTile = &(Tile::CreateTileN(1, startID, PreyType::None)[0]);
    currentTile->setRotation(2);
    testingTilePlacement(&startID, 73, 73, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTileID = currentTile->getId();
    currentSet = game->regions->getRegions(currentTileID);

    actualScore = game->rules->scoreCastle(*currentSet, true, false);
    EXPECT_EQ(actualScore,24);
    actualScore = game->rules->scoreCastle(*currentSet, false, false);
    EXPECT_EQ(actualScore,24);
}
*/
TEST_F(BoardRulesTests, scoreRoad)
{
    unsigned int startID = 0;
    game->boardManager->gameInit();
    Tile *currentTile;
    //Move *currentMove;
    //Coord *currentCoord;
    const Tile **surroundingTiles;

    /*currentTile = &(Tile::CreateTileD(1, startID, PreyType::Deer)[0]);
    unsigned int currentTileID = currentTile->getId();
    currentTile->setRotation(0);
    testingTilePlacement(&startID, 72, 72, currentTile, surroundingTiles);
    currentTile->placeTile();

    */
    std::shared_ptr<struct regionSet> currentSet = std::shared_ptr<struct regionSet>(new regionSet);
    std::shared_ptr<struct tileNode> currentTileNode = std::shared_ptr<struct tileNode>(new tileNode);
    currentSet->head = currentTileNode;

    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < NUM_PREY; j++)
        {
            currentTileNode->preyCounts[j] = 0;
        }
        currentTileNode->tileID = i;
        currentTileNode->edge = i;
        if(i+1 < 5)
        {
            currentTileNode->next = std::shared_ptr<struct tileNode>(new tileNode);
            currentTileNode = currentTileNode->next;
        }
    }
    currentSet->tail = currentTileNode;
    currentTileNode->next = NULL;

    currentSet->edgesTillCompletion = 1;

    // not sure what to pass through for bool actuallyScore
    unsigned int returnScore = game->rules->scoreRoad(currentSet, true);
    EXPECT_EQ(returnScore, 0);
    returnScore = game->rules->scoreRoad(currentSet, false);
    EXPECT_EQ(returnScore, 5);


    currentTileNode->next = std::shared_ptr<struct tileNode>(new tileNode);
    currentTileNode = currentTileNode->next;
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < NUM_PREY; j++)
        {
            currentTileNode->preyCounts[j] = 1;
        }
        currentTileNode->tileID = i+5;
        currentTileNode->edge = i;
        if(i+1 < 5)
        {
            currentTileNode->next = std::shared_ptr<struct tileNode>(new tileNode);
            currentTileNode = currentTileNode->next;
        }
    }
    currentSet->tail = currentTileNode;
    currentTileNode->next = NULL;

    currentSet->edgesTillCompletion = 0;


    returnScore = game->rules->scoreRoad(currentSet, false);
    EXPECT_EQ(returnScore, 20);
    returnScore = game->rules->scoreRoad(currentSet, false);
    EXPECT_EQ(returnScore, 20);


}
/*
TEST_F(BoardRulesTests, scoreGrassAndRoad)
{
    //Testing grass next to completed castle/lake and incomplete den
    //Testing complete road that starts with den and ends with castle
    unsigned int startID = 0;
    Tile *currentTile;
    //Move *currentMove;
    //Coord *currentCoord;
    const Tile **surroundingTiles;

    //this creates a complete castle, so grass should be 5 pts
    currentTile = &(Tile::CreateTileZ(1, startID, PreyType::None)[0]);
    currentTile->setRotation(2);
    testingTilePlacement(&startID, 72, 71, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileG(1, startID, PreyType::None)[0]);
    currentTile->setRotation(1);
    testingTilePlacement(&startID, 72, 70, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileE(1, startID, PreyType::None)[0]);
    currentTile->setRotation(0);
    testingTilePlacement(&startID, 72, 69, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileA(1, startID, PreyType::None)[0]);
    currentTile->setRotation(0);
    testingTilePlacement(&startID, 72, 72, currentTile, surroundingTiles);
    currentTile->placeTile();

    unsigned int currentTileID = currentTile->getId();
    std::shared_ptr<struct regionSet> *currentSet = game->regions->getRegions(currentTileID);

    // get score for Grass in this set of 4 tiles that is placed together
    unsigned int returnGrassScore = game->rules->scoreGrass(currentSet, currentTileID, 10);
   // get score for the completed road  in this set of 4 tiles placed together
    unsigned int returnRoadScore = game->rules->scoreRoad(*currentSet, true);
    unsigned int realGrassScore = 5;
    unsigned int realRoadScore = 2;

    EXPECT_EQ(realGrassScore, returnGrassScore);
    EXPECT_EQ(realRoadScore, returnRoadScore);
}
*/
TEST_F(BoardRulesTests, scoreGrass)
{
    //Testing grass next to completed den and completed lake
    unsigned int startID = 18000;
    //Tile *currentTile;
    //Move *currentMove;
    //Coord *currentCoord;
    const Tile **surroundingTiles;

    game->boardManager->gameInit();
    //printf("tile\n");
    Tile tileV = (Tile::CreateTileV(1, startID, PreyType::None)[0]); //No prey, starting tile
    Tile * currentTile = &tileV;
    //testingTilePlacement(&startID, 72, 73, currentTile, surroundingTiles);

    game->boardManager->makeMove(Move(*currentTile, Coord(76, 75)), 1);
    //printf("tile\n");

    Tile tileX = (Tile::CreateTileX(1, startID, PreyType::None)[0]); //No prey, starting tile
    currentTile = &tileX;
    //testingTilePlacement(&startID, 72, 73, currentTile, surroundingTiles);


    game->boardManager->makeMove(Move(*currentTile, Coord(75, 75)), 1);
    //printf("tile\n");

    Tile tileU = (Tile::CreateTileU(1, startID, PreyType::None)[0]); //No prey, starting tile
    currentTile = &tileU;
    //testingTilePlacement(&startID, 72, 73, currentTile, surroundingTiles);

    game->boardManager->makeMove(Move(*currentTile, Coord(75, 76)), 1);
    //printf("tile\n");


    Tile tileT = (Tile::CreateTileT(1, startID, PreyType::None)[0]); //No prey, starting tile
    currentTile = &tileT;
    //testingTilePlacement(&startID, 72, 73, currentTile, surroundingTiles);

    game->boardManager->makeMove(Move(*currentTile, Coord(76, 75), 2), 1);
    //printf("tile\n");


    Tile tileP = (Tile::CreateTileP(1, startID, PreyType::None)[0]); //No prey, starting tile
    currentTile = &tileP;
    //testingTilePlacement(&startID, 72, 73, currentTile, surroundingTiles);

    game->boardManager->makeMove(Move(*currentTile, Coord(76, 77), 2), 1);
    //printf("tile\n");


    Tile tileD2 = (Tile::CreateTileD(1, startID, PreyType::None)[0]); //No prey, starting tile
    currentTile = &tileD2;
    //testingTilePlacement(&startID, 72, 73, currentTile, surroundingTiles);

    game->boardManager->makeMove(Move(*currentTile, Coord(76, 75)), 1);
    //printf("tile\n");


    Tile tileD = (Tile::CreateTileD(1, startID, PreyType::None)[0]); //No prey, starting tile
    currentTile = &tileD;
    //testingTilePlacement(&startID, 72, 73, currentTile, surroundingTiles);

    game->boardManager->makeMove(Move(*currentTile, Coord(77, 77), 3), 1);
    //printf("tile\n");


    Tile tileN = (Tile::CreateTileN(1, startID, PreyType::None)[0]); //No prey, starting tile
    currentTile = &tileN;
    //testingTilePlacement(&startID, 72, 73, currentTile, surroundingTiles);

    game->boardManager->makeMove(Move(*currentTile, Coord(77, 76), 2), 1);
    //printf("tile\n");


    Tile tileZ = (Tile::CreateTileZ(1, startID, PreyType::None)[0]); //No prey, starting tile
    currentTile = &tileZ;
    //testingTilePlacement(&startID, 72, 73, currentTile, surroundingTiles);
    unsigned int grassGrass = tileZ.getId();

    game->boardManager->makeMove(Move(*currentTile, Coord(77, 75)), 1);
    //printf("tile\n");



    std::shared_ptr<struct regionSet> *currentSets = game->regions->regionTracker[grassGrass];
    //printf("sets?\n");
    unsigned int returnGrassScore = game->rules->scoreGrass(currentSets, grassGrass, 10);
    unsigned int realGrassScore = 3; //5 pt for completed lake + 3 pts for completed den

                                EXPECT_EQ(returnGrassScore, 0);
}

/*
TEST_F(BoardRulesTests, scoreMoreRoads)
{
    //Testing incomplete road with prey animals
    unsigned int startID = 0;
    Tile *currentTile;
    //Move *currentMove;
    //Coord *currentCoord;
    const Tile **surroundingTiles;

    currentTile = &(Tile::CreateTileZ(1, startID, PreyType::Deer)[0]);
    currentTile->setRotation(3);
    testingTilePlacement(&startID, 72, 72, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileK(1, startID, PreyType::Boar)[0]);
    currentTile->setRotation(0);
    testingTilePlacement(&startID, 73, 72, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileU(1, startID, PreyType::None)[0]);
    currentTile->setRotation(1);
    testingTilePlacement(&startID, 74, 73, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileJ(1, startID, PreyType::Buffalo)[0]);
    currentTile->setRotation(3);
    testingTilePlacement(&startID, 73, 73, currentTile, surroundingTiles);
    currentTile->placeTile();

    unsigned int currentTileID = currentTile->getId();

    std::shared_ptr<struct regionSet> *currentSet = game->regions->getRegions(currentTileID);

    unsigned int returnRoadScore = game->rules->scoreRoad(*currentSet, false);

    unsigned int realRoadScore = 7;

    EXPECT_EQ(realRoadScore, returnRoadScore);
}


TEST (BoardRulesTests, scoreMoreMoreRoads)
{
    //Testing complete road that starts with den and ends with den and has prey animals
    unsigned int startID = 0;
    Tile *currentTile;
    //Move *currentMove;
    //Coord *currentCoord;
    const Tile **surroundingTiles;

    currentTile = &(Tile::CreateTileA(1, startID, PreyType::None)[0]);
    currentTile->setRotation(3);
    testingTilePlacement(&startID, 72, 72, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileJ(1, startID, PreyType::Buffalo)[0]);
    currentTile->setRotation(0);
    testingTilePlacement(&startID, 73, 72, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileV(1, startID, PreyType::None)[0]);
    currentTile->setRotation(1);
    testingTilePlacement(&startID, 73, 71, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileA(1, startID, PreyType::None)[0]);
    currentTile->setRotation(1);
    testingTilePlacement(&startID, 72, 72, currentTile, surroundingTiles);
    currentTile->placeTile();

    unsigned int currentTileID = currentTile->getId();

    std::shared_ptr<struct regionSet> *currentSet = game->regions->getRegions(currentTileID);

    unsigned int returnRoadScore = game->rules->scoreRoad(*currentSet, false);

    unsigned int realRoadScore = 5;

    EXPECT_EQ(realRoadScore, returnRoadScore);
}

TEST_F(BoardRulesTests, scoreGrassWithJustCompleteDen)
{
    //Testing grass next to completed den but incomplete lake
    //Testing two different incomplete sets of roads
    unsigned int startID = 0;
    Tile *currentTile;
    //Move *currentMove;
    //Coord *currentCoord;
    const Tile **surroundingTiles;

    currentTile = &(Tile::CreateTileJ(1, startID, PreyType::Buffalo)[0]);
    currentTile->setRotation(3);
    testingTilePlacement(&startID, 72, 72, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileV(1, startID, PreyType::None)[0]);
    currentTile->setRotation(0);
    testingTilePlacement(&startID, 73,72, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileA(1, startID, PreyType::None)[0]);
    currentTile->setRotation(1);
    testingTilePlacement(&startID, 72, 72, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileU(1, startID, PreyType::None)[0]);
    currentTile->setRotation(0);
    testingTilePlacement(&startID, 73,70, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileU(1, startID, PreyType::None)[0]);
    currentTile->setRotation(0);
    testingTilePlacement(&startID, 72,70, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileU(1, startID, PreyType::None)[0]);
    currentTile->setRotation(1);
    testingTilePlacement(&startID, 74,72, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileV(1, startID, PreyType::None)[0]);
    currentTile->setRotation(0);
    testingTilePlacement(&startID, 74,70, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTile = &(Tile::CreateTileD(1, startID, PreyType::Deer)[0]);
    currentTile->setRotation(0);
    testingTilePlacement(&startID, 74,71, currentTile, surroundingTiles);
    currentTile->placeTile();

    //Testing road portion of this landscape
    unsigned int currentTileId = currentTile->getId();
    std::shared_ptr<struct regionSet> *currentSet = game->regions->getRegions(currentTileId);

    unsigned int returnRoadScore = game->rules->scoreRoad(*currentSet, false);

    unsigned int realRoadScore = 6;

    EXPECT_EQ(realRoadScore, returnRoadScore);

    currentTile = &(Tile::CreateTileV(1, startID, PreyType::None)[0]);
    currentTile->setRotation(1);
    testingTilePlacement(&startID, 72, 71, currentTile, surroundingTiles);
    currentTile->placeTile();

    unsigned int currentTileID = currentTile->getId();
    std::shared_ptr<struct regionSet> *currentSet2 = game->regions->getRegions(currentTileID);

    unsigned int returnRoadScore2 = game->rules->scoreRoad(*currentSet2, false);
    unsigned int returnGrassScore = game->rules->scoreGrass(currentSet2, currentTileID, 7);
    unsigned int realRoadScore2 = 5;
    unsigned int realGrassScore = 3;
    EXPECT_EQ(realRoadScore2, returnRoadScore2);
    EXPECT_EQ(realGrassScore, returnGrassScore);

}

TEST_F(BoardRulesTests, getCurrentScore) {
    unsigned int startID = 0;
    Tile *currentTile;
    //Move *currentMove;
    //Coord *currentCoord;
    const Tile **surroundingTiles;

    currentTile = &(Tile::CreateTileD(1, startID, PreyType::None)[0]);
    currentTile->setRotation(0);
    testingTilePlacement(&startID, 72, 72, currentTile, surroundingTiles);
    currentTile->placeTile();

    unsigned int currentTileId = currentTile->getId();
    std::shared_ptr<struct regionSet> *currentSet = game->regions->getRegions(currentTileId);

    unsigned int edge = 4; // uncompleted city on right side of tile
    unsigned int tilesSurrounded = game->boardManager->isSurrounded(currentTileId);

    unsigned int actualScore = game->rules->getCurrentScore(currentSet, edge, currentTile, tilesSurrounded);
    unsigned int expectedScore = 1; // uncompleted city on right side of tile

    EXPECT_EQ(actualScore, expectedScore);

    currentTile = &(Tile::CreateTileJ(1, startID, PreyType::Buffalo)[0]);
    currentTile->setRotation(2);
    testingTilePlacement(&startID, 73, 72, currentTile, surroundingTiles);
    currentTile->placeTile();

    currentTileId = currentTile->getId();
    currentSet = game->regions->getRegions(currentTileId);
    edge = 10; // test left side of tile
    tilesSurrounded = game->boardManager->isSurrounded(currentTileId);

    actualScore = game->rules->getCurrentScore(currentSet, edge, currentTile, tilesSurrounded);
    expectedScore = 8; // 8 points for a completed city adjacent to 1 unique prey

    EXPECT_EQ(actualScore, expectedScore);
}
*/
    

TEST_F(BoardRulesTests, validTilePlacement)
{
    game->boardManager->gameInit();

    unsigned int tileIdCounter = 100;


    Tile tile = Tile::CreateTileJ(1, tileIdCounter, PreyType::None)[0];


    const Coord left = Coord(75, 76);
    const Coord right = Coord(77, 76);
    const Coord up = Coord(76, 77);
    const Coord down = Coord(76, 75);

    tile.setRotation(0);
    std::cout << std::endl << "left ";
    if(game->rules->validTilePlacement(tile, game->board->getBorderingTiles(left))) { std::cout <<  "0 "; }
    tile.setRotation(1);
    if(game->rules->validTilePlacement(tile, game->board->getBorderingTiles(left))) { std::cout <<  "1 "; }
    tile.setRotation(2);
    if(game->rules->validTilePlacement(tile, game->board->getBorderingTiles(left))) { std::cout <<  "2 "; }
    tile.setRotation(3);
    if(game->rules->validTilePlacement(tile, game->board->getBorderingTiles(left))) { std::cout <<  "3 "; }

    tile.setRotation(0);
    std::cout << std::endl << "right ";
    if(game->rules->validTilePlacement(tile, game->board->getBorderingTiles(right))) { std::cout <<  "0 "; }
    tile.setRotation(1);
    if(game->rules->validTilePlacement(tile, game->board->getBorderingTiles(right))) { std::cout <<  "1 "; }
    tile.setRotation(2);
    if(game->rules->validTilePlacement(tile, game->board->getBorderingTiles(right))) { std::cout <<  "2 "; }
    tile.setRotation(3);
    if(game->rules->validTilePlacement(tile, game->board->getBorderingTiles(right))) { std::cout <<  "3 "; }

    tile.setRotation(0);
    std::cout << std::endl << "up ";
    if(game->rules->validTilePlacement(tile, game->board->getBorderingTiles(up))) { std::cout <<  "0 "; }
    tile.setRotation(1);
    if(game->rules->validTilePlacement(tile, game->board->getBorderingTiles(up))) { std::cout <<  "1 "; }
    tile.setRotation(2);
    if(game->rules->validTilePlacement(tile, game->board->getBorderingTiles(up))) { std::cout <<  "2 "; }
    tile.setRotation(3);
    if(game->rules->validTilePlacement(tile, game->board->getBorderingTiles(up))) { std::cout <<  "3 "; }

    tile.setRotation(0);
    std::cout << std::endl << "down ";
    if(game->rules->validTilePlacement(tile, game->board->getBorderingTiles(down))) { std::cout <<  "0 "; }
    tile.setRotation(1);
    if(game->rules->validTilePlacement(tile, game->board->getBorderingTiles(down))) { std::cout <<  "1 "; }
    tile.setRotation(2);
    if(game->rules->validTilePlacement(tile, game->board->getBorderingTiles(down))) { std::cout <<  "2 "; }
    tile.setRotation(3);
    if(game->rules->validTilePlacement(tile, game->board->getBorderingTiles(down))) { std::cout <<  "3 "; }
    std::cout << std::endl;

    /*
    Tile tile2 = Tile::CreateTileV(1, tileIdCounter, PreyType::None)[0];
    Coord coord2 = Coord(77, 75);
    Move move2 = Move(tile2, coord2);

    

    Tile tile3 = Tile::CreateTileK(1, tileIdCounter, PreyType::None)[0];
    Coord coord3 = Coord(77, 76);
    Move move3 = Move(tile3, coord3, 2);*/
}

};