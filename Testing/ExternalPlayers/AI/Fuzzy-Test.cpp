#include "../ExternalPlayers/AI/FuzzyLogic.h"
#include "../ExternalPlayers/AI/AI.h"
#include "../Core/BoardManager/BoardManager.h"
#include "gtest/gtest.h"

namespace {

class AITests : public ::testing::Test {

 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  AITests() {
    // You can do set-up work for each test here.
  }

  virtual ~AITests() {
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

TEST_F(AITests, getTurnScore)
{
    //I convert the expected and return values to ints to account for some loss in accuracy in ownership
    struct Graph g;
    g.leftStart = 0;
    g.leftEnd = 3;
    g.midStart = 0;
    g.midPlatStart = 3;
    g.midPlatEnd = 4;
    g.midEnd = 10;
    g.rightStart = 4;
    g.rightEnd = 10;
    FuzzySet fs(HURTING_ENEMY, "HURTING_ENEMY", g);

    float expectedScore;

    fs.enterData(0);
    expectedScore = (100)*(HURTING_GOOD_WHT);
    EXPECT_EQ((int)fs.getTurnScore(), (int) expectedScore);

    fs.enterData(3);
    expectedScore = (100)*(HURTING_NEUTRAL_WHT);
    EXPECT_EQ((int)fs.getTurnScore(), (int) expectedScore);

    fs.enterData(10);
    expectedScore = (100)*(HURTING_BAD_WHT);
    EXPECT_EQ((int)fs.getTurnScore(), (int) expectedScore);

    fs.enterData(2);
    expectedScore = (200.f/3.f) * (float)HURTING_NEUTRAL_WHT + (100.f/3.f) * (float)HURTING_GOOD_WHT;
    EXPECT_EQ((int)fs.getTurnScore(), (int) expectedScore);

    fs.enterData(5);
    //printf("Mid ownership: %4.4f Right ownership: %4.4f\n", (100.f - (100.f/6.f)), (100.f/6.f));
    expectedScore = (100.f - (100.f/6.f)) * (float)HURTING_NEUTRAL_WHT +  (100.f/6.f) * (float)HURTING_BAD_WHT;
    EXPECT_EQ((int)fs.getTurnScore(), (int) expectedScore);

    g = Graph();
    g.leftStart = 0;
    g.leftEnd = 3;
    g.midStart = 0;
    g.midPlatStart = 3;
    g.midPlatEnd = 4;
    g.midEnd = 10;
    g.rightStart = 4;
    g.rightEnd = 10;
    fs = FuzzySet(HELPING_ME, "HELPING_ME", g);

    fs.enterData(0);
    expectedScore = (100)*(HELPING_BAD_WHT);
    EXPECT_EQ((int)fs.getTurnScore(), (int) expectedScore);

    fs.enterData(3);
    expectedScore = (100)*(HELPING_NEUTRAL_WHT);
    EXPECT_EQ((int)fs.getTurnScore(), (int) expectedScore);

    fs.enterData(10);
    expectedScore = (100)*(HELPING_GOOD_WHT);
    EXPECT_EQ((int)fs.getTurnScore(), (int) expectedScore);

    fs.enterData(2);
    expectedScore = (200.f/3.f) * (float)HELPING_NEUTRAL_WHT + (100.f/3.f) * (float)HELPING_BAD_WHT;
    EXPECT_EQ((int)fs.getTurnScore(), (int) expectedScore);

    fs.enterData(5);
    expectedScore = (100.f - (100.f/6.f)) * (float)HELPING_NEUTRAL_WHT + (100.f/6.f) * (float)HELPING_GOOD_WHT;
    EXPECT_EQ((int)fs.getTurnScore(), (int) expectedScore);

    g = Graph();
    g.leftStart = 0;
    g.leftEnd = 3;
    g.midStart = 0;
    g.midPlatStart = 3;
    g.midPlatEnd = 4;
    g.midEnd = 6;
    g.rightStart = 4;
    g.rightEnd = 6;
    fs = FuzzySet(TURNS_AWAY, "TURNS_AWAY", g);

    fs.enterData(0);
    expectedScore = (100)*(TURNS_SHORT_WHT);
    EXPECT_EQ((int)fs.getTurnScore(), (int) expectedScore);

    fs.enterData(3);
    expectedScore = (100)*(TURNS_MEDIUM_WHT);
    EXPECT_EQ((int)fs.getTurnScore(), (int) expectedScore);

    fs.enterData(10);
    expectedScore = (100)*(TURNS_LONG_WHT);
    EXPECT_EQ((int)fs.getTurnScore(), (int) expectedScore);

    fs.enterData(2);
    expectedScore = (200.f/3.f) * (float)TURNS_MEDIUM_WHT + (100.f/3.f) * (float)TURNS_SHORT_WHT;
    EXPECT_EQ((int)fs.getTurnScore(), (int) expectedScore);

    fs.enterData(5);
    expectedScore = (50.f) * (float)TURNS_MEDIUM_WHT + (50.f) * (float)TURNS_LONG_WHT;
    EXPECT_EQ((int)fs.getTurnScore(), (int) expectedScore);
}

TEST_F(AITests, getResults)
{
    FuzzyLogic fz;
    struct AIMove mv;

    float expectedScore;

    //All LEFT
    mv.edgesTillCompletion = 0;
    mv.diffMyScore = 0;
    mv.diffEnemyScore = 0;

    fz.enterData(mv);
    expectedScore = (100) * TURNS_SHORT_WHT + (100) * HELPING_BAD_WHT + (100) * HURTING_GOOD_WHT;
    EXPECT_EQ((int) fz.getResults(), (int) expectedScore);

    //All MID
    mv.edgesTillCompletion = 0;
    mv.diffMyScore = 0;
    mv.diffEnemyScore = 0;

    fz.enterData(mv);
    expectedScore = (100) * TURNS_SHORT_WHT + (100) * HELPING_BAD_WHT + (100) * HURTING_GOOD_WHT;
    EXPECT_EQ((int) fz.getResults(), (int) expectedScore);

    //All RIGHT
    mv.edgesTillCompletion = 20;
    mv.diffMyScore = 20;
    mv.diffEnemyScore = 20;

    fz.enterData(mv);
    expectedScore = (100) * TURNS_LONG_WHT + (100) * HELPING_GOOD_WHT + (100) * HURTING_BAD_WHT;
    EXPECT_EQ((int) fz.getResults(), (int) expectedScore);

    //LEFT, MID, RIGHT
    mv.edgesTillCompletion = 0;
    mv.diffMyScore = 3;
    mv.diffEnemyScore = 20;

    fz.enterData(mv);
    expectedScore = (100) * TURNS_SHORT_WHT + (100) * HELPING_NEUTRAL_WHT + (100) * HURTING_BAD_WHT;
    EXPECT_EQ((int) fz.getResults(), (int) expectedScore);

    //RIGHT, LEFT, MID
    mv.edgesTillCompletion = 20;
    mv.diffMyScore = 0;
    mv.diffEnemyScore = 20;

    fz.enterData(mv);
    expectedScore = (100) * TURNS_LONG_WHT + (100) * HELPING_BAD_WHT + (100) * HURTING_BAD_WHT;
    EXPECT_EQ((int) fz.getResults(), (int) expectedScore);


    //All MIXED
    mv.edgesTillCompletion = 2;
    mv.diffMyScore = 5;
    mv.diffEnemyScore = 2;

    fz.enterData(mv);
    expectedScore = (200.f/3.f) * (float)TURNS_MEDIUM_WHT + (100.f/3.f) * (float)TURNS_SHORT_WHT +
                    (100.f - (100.f/6.f)) * (float)HELPING_NEUTRAL_WHT + (100.f/6.f) * (float)HELPING_GOOD_WHT +
                    (200.f/3.f) * (float)HURTING_NEUTRAL_WHT + (100.f/3.f) * (float)HURTING_GOOD_WHT;
    EXPECT_EQ((int) fz.getResults(), (int) expectedScore);
}


TEST_F(AITests, setPlayerNumber)
{
    game->ai->setPlayerNumber(0);
    EXPECT_EQ(game->ai->myPlayerNumber, (unsigned int)0);

    game->ai->setPlayerNumber(1);
    EXPECT_EQ(game->ai->myPlayerNumber, (unsigned int)1);

    game->ai->setPlayerNumber(2);
    EXPECT_EQ(game->ai->myPlayerNumber, (unsigned int)2);
}

TEST_F(AITests, chooseTurn)
{
    game->ai->setPlayerNumber(1);
    Move chosenMove = game->ai->chooseTurn(game->boardManager->getTopTileStack());
    EXPECT_EQ(chosenMove.getCoord().getX(), 9);
}

};