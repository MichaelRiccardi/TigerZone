#include "../Core/Board/Board.h"
#include "../Core/BoardManager/Move.h"
#include "../Core/BoardManager/Coord.h"
#include "../TigerZoneGame/TigerZoneGame.h"

#include "gtest/gtest.h"
#define GRID_SIZE 153


namespace {

// The fixture for testing class Foo.
class BoardTests : public ::testing::Test {

 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  BoardTests() {
    // You can do set-up work for each test here.
  }

  virtual ~BoardTests() {
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


TEST_F(BoardTests, getBoard)
{


    const Array<Array<Tile*>>& boardGrid = game->board->getBoard();

    EXPECT_EQ(boardGrid.getSize(), (unsigned int)GRID_SIZE);

    for(unsigned int i = 0; i < (unsigned int)GRID_SIZE; i++)
    {
        EXPECT_EQ(boardGrid[i].getSize(), (unsigned int)GRID_SIZE);

        for(unsigned int j = 0; j < (unsigned int)GRID_SIZE; j++)
        {
            EXPECT_EQ(nullptr, boardGrid[i][j]);
        }
    }
}

TEST_F(BoardTests, place)
{
    

    const Array<Array<Tile*>>& boardGrid = game->board->getBoard();

    unsigned int tileIDcounter = 0;
    Tile tile0 = Tile::CreateTileA(1, tileIDcounter, PreyType::None)[0];
    Tile tile1 = Tile::CreateTileB(1, tileIDcounter, PreyType::None)[0];
    Tile tile2 = Tile::CreateTileC(1, tileIDcounter, PreyType::None)[0];

    const Move& move0 = Move(tile0, 0, 0);
    const Move& move1 = Move(tile1, 75, 76, 2);
    const Move& move2 = Move(tile2, 152, 152);

    for(unsigned int m = 0; m <= 3; m++)
    {
        for(unsigned int i = 0; i < GRID_SIZE; i++)
        {
            for(unsigned int j = 0; j < GRID_SIZE; j++)
            {
                if(m > 0 && i == 0 && j == 0)
                {
                    EXPECT_EQ(&tile0, boardGrid[i][j]);
                }
                else if(m > 1 && i == 75 && j == 76)
                {
                    EXPECT_EQ(&tile1, boardGrid[i][j]);
                }
                else if(m > 2 && i == 152 && j == 152)
                {
                    EXPECT_EQ(&tile2, boardGrid[i][j]);
                }
                else // expect all other locations to be empty
                {
                    EXPECT_EQ(nullptr, boardGrid[i][j]); 
                }
            }
        }
        switch(m)
        {
            case 0: game->board->place(move0); return;
            case 1: game->board->place(move1); return;
            case 2: game->board->place(move2); return;
        }
    }
}

TEST_F(BoardTests, getFromCoord)
{
    

    unsigned int tileIdCounter = 0;
    Tile tile = Tile::CreateTileD(1, tileIdCounter, PreyType::None)[0];
    const Coord& coord = Coord(10, 20);
    const Move& move = Move(tile, coord);
    game->board->place(move);

    EXPECT_EQ(game->board->get(coord), &tile);
}

TEST_F(BoardTests, getFromTileId)
{
    

    unsigned int tileIdCounter = 5;
    Tile tile = Tile::CreateTileD(1, tileIdCounter, PreyType::None)[0];
    const Move& move = Move(tile, 76, 76);
    game->board->place(move);

    EXPECT_EQ(game->board->get((unsigned int) 5), &tile);
}

TEST_F(BoardTests, getBorderingTiles)
{
    

    unsigned int tileIDcounter = 0;

    Tile tileCenter = Tile::CreateTileI(1, tileIDcounter, PreyType::None)[0];
    Tile tile0 = Tile::CreateTileA(1, tileIDcounter, PreyType::None)[0];
    Tile tile1 = Tile::CreateTileB(1, tileIDcounter, PreyType::None)[0];
    Tile tile2 = Tile::CreateTileC(1, tileIDcounter, PreyType::None)[0];
    Tile tile3 = Tile::CreateTileD(1, tileIDcounter, PreyType::None)[0];
    Tile tile4 = Tile::CreateTileE(1, tileIDcounter, PreyType::None)[0];
    Tile tile5 = Tile::CreateTileG(1, tileIDcounter, PreyType::None)[0];
    Tile tile6 = Tile::CreateTileH(1, tileIDcounter, PreyType::None)[0];

    unsigned int i = 0;

    int dx[8] = { -1, 0, 1, 1, 1, 0, -1, -1 }; 
    int dy[8] = { 1, 1, 1, 0, -1, -1, -1, 0 };

    const Move& moveCenter = Move(tileCenter, 1, 1);
    const Move& move0 = Move(tile0, 1 + dx[i], 1 + dy[i]); i++;
    const Move& move1 = Move(tile1, 1 + dx[i], 1 + dy[i]);  i++;
    const Move& move2 = Move(tile2, 1 + dx[i], 1 + dy[i]);  i++;
    const Move& move3 = Move(tile3, 1 + dx[i], 1 + dy[i]);  i++;
    const Move& move4 = Move(tile4, 1 + dx[i], 1 + dy[i]);  i++;
    const Move& move5 = Move(tile5, 1 + dx[i], 1 + dy[i]);  i++;
    const Move& move6 = Move(tile6, 1 + dx[i], 1 + dy[i]); 

    game->board->place(moveCenter);
    game->board->place(move0);
    game->board->place(move1);
    game->board->place(move2);
    game->board->place(move3);
    game->board->place(move4);
    game->board->place(move5);
    game->board->place(move6);

    const Tile** borderingTiles = game->board->getBorderingTiles(tileCenter);

    EXPECT_EQ(borderingTiles[0], &tile0);
    EXPECT_EQ(borderingTiles[1], &tile1);
    EXPECT_EQ(borderingTiles[2], &tile2);
    EXPECT_EQ(borderingTiles[3], &tile3);
    EXPECT_EQ(borderingTiles[4], &tile4);
    EXPECT_EQ(borderingTiles[5], &tile5);
    EXPECT_EQ(borderingTiles[6], &tile6);
    EXPECT_EQ(borderingTiles[7], nullptr); // left is empty
}

TEST_F(BoardTests, getCoordinatesFromTileId)
{
    

    unsigned int tileIdCounter = 5;
    int x = 10;
    int y = 20;

    Tile tile = Tile::CreateTileD(1, tileIdCounter, PreyType::None)[0];
    const Coord& coord = Coord(x, y);
    const Move& move = Move(tile, coord);
    game->board->place(move);

    const Coord& result = game->board->getCoordinatesFromTileId((unsigned int) 5);

    EXPECT_EQ(result.getX(), x);
    EXPECT_EQ(result.getY(), y);
}

TEST_F(BoardTests, getCoordinatesFromGridId)
{
    

    int x = 10;
    int y = 20;
    int gridId = y * GRID_SIZE + x;

    const Coord& result = game->board->getCoordinatesFromGridId(gridId);

    EXPECT_EQ(result.getX(), x);
    EXPECT_EQ(result.getY(), y);
}

TEST_F(BoardTests, getGridId)
{
    

    int x = 10;
    int y = 20;
    int gridId = y * GRID_SIZE + x;

    const Coord& coord = Coord(x, y);

    EXPECT_EQ(game->board->getGridId(coord), (unsigned int)gridId);
}


TEST_F(BoardTests, getAvailableLocations)
{
    

    unsigned int tileIDcounter = 0;
    Tile tile0 = Tile::CreateTileD(1, tileIDcounter, PreyType::None)[0];
    Tile tile1 = Tile::CreateTileJ(1, tileIDcounter, PreyType::Buffalo)[0];
    Tile tile2 = Tile::CreateTileV(1, tileIDcounter, PreyType::None)[0];

    const Move& move0 = Move(tile0, 76, 76);
    const Move& move1 = Move(tile1, 76, 75, 2); // rotate 180 degrees
    const Move& move2 = Move(tile2, 77, 75);

    const std::unordered_set<unsigned int>& availableLocations = game->board->getAvailableLocations();

    EXPECT_EQ(availableLocations.size(), (unsigned int)0);

    game->board->place(move0);

        const unsigned int size0 = 4;
        EXPECT_EQ(availableLocations.size(), size0);

        unsigned int x0[size0] = { 76, 75, 77, 76 };
        unsigned int y0[size0] = { 77, 76, 76, 75 };

        for(unsigned int i = 0; i < size0; i++)
        {
            EXPECT_GT(availableLocations.count(game->board->getGridId(x0[i], y0[i])), (unsigned int)0);
        }

    game->board->place(move1);

        const unsigned int size1 = 6;
        EXPECT_EQ(availableLocations.size(), size1);

        unsigned int x1[size1] = { 76, 75, 77, 75, 77, 76 };
        unsigned int y1[size1] = { 77, 76, 76, 75, 75, 74 };

        for(unsigned int i = 0; i < size1; i++)
        {
            EXPECT_GT(availableLocations.count(game->board->getGridId(x1[i], y1[i])), (unsigned int)0);
        }

    game->board->place(move2);
    
        const unsigned int size2 = 7;
        EXPECT_EQ(availableLocations.size(), size2);

        unsigned int x2[size2] = { 76, 75, 77, 75, 76, 78, 77 };
        unsigned int y2[size2] = { 77, 76, 76, 75, 74, 75, 74 };

        for(unsigned int i = 0; i < size2; i++)
        {
            EXPECT_GT(availableLocations.count(game->board->getGridId(x2[i], y2[i])), (unsigned int)0);
        }
}

};

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}