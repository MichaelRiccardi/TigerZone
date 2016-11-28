#include "../Core/Board/Board.h"
#include "../Core/BoardManager/Move.h"
#include "../Core/BoardManager/Coord.h"
#include "../Core/BoardManager/BoardManager.h"
#include "gtest/gtest.h"
#define GRID_SIZE 153

TEST(BoardManagerTests, getBoard)
{
    BoardManager::gameInit();
    EXPECT_EQ(&BoardManager::getBoard(), &Board::getBoard());
}

TEST(BoardManagerTests, gameInit)
{
    BoardManager::gameInit();

    const Array<Array<Tile*>>& boardGrid = BoardManager::getBoard();

    for(unsigned int i = 0; i < GRID_SIZE; i++)
    {
        for(unsigned int j = 0; j < GRID_SIZE; j++)
        {
            if(i == 76 && j == 76) // center
            {
                Tile* tile = boardGrid[i][j];
                EXPECT_EQ(tile->getTileType(), TileType::D);

////////////////FLOATING POINT EXCEPTION vvvv
                EXPECT_EQ(tile->getRotation(), (unsigned int)0);
////////////////FLOATING POINT EXCEPTION ^^^^

                EXPECT_EQ(tile->getPrey(), PreyType::None);
                EXPECT_EQ(tile->isPlaced(), true);

            }
            else // expect all other locations to be empty
            {
                EXPECT_EQ(nullptr, boardGrid[i][j]); 
            }
        }
    }
}

TEST(BoardManagerTests, getTileStack)
{
    BoardManager::gameInit();
    int actualTileCounts[NUM_TILES]   = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    int expectedTileCounts[NUM_TILES] = { 2,4,1,3,2,5,3,3,2,1,2,1,2,1,2,5,3,2,4,1,2,8,9,4,1,1,1,2 };
    expectedTileCounts[3]--; // starting Tile

    TileStack* tileStack = BoardManager::getTileStack();
    std::queue<Tile*> tileQueue1 = tileStack->getQueue((unsigned int) 1);
    std::queue<Tile*> tileQueue2 = tileStack->getQueue((unsigned int) 2);

    EXPECT_EQ(tileQueue1.size(), (unsigned int) (NUMBER_OF_PLAYABLE_TILES / 2));
    EXPECT_EQ(tileQueue2.size(), (unsigned int) (NUMBER_OF_PLAYABLE_TILES / 2));

    while(!tileQueue1.empty() || !tileQueue2.empty())
    {
        const Tile* next;
        if(!tileQueue1.empty())
        {
            next = tileQueue1.front();
            tileQueue1.pop();
        }
        else
        {
            next = tileQueue2.front();
            tileQueue2.pop();
        }

        switch(next->getTileType())
        {
            case TileType::A : actualTileCounts[0]++; break;
            case TileType::B : actualTileCounts[1]++; break;
            case TileType::C : actualTileCounts[2]++; break;
            case TileType::D :
                 if(next->getPrey() == PreyType::None)
                 {
                    actualTileCounts[3]++;
                 }
                 else if(next->getPrey() == PreyType::Deer)
                 {
                    actualTileCounts[4]++;
                 }
                 break;
            case TileType::E : actualTileCounts[5]++; break;
            case TileType::G : actualTileCounts[6]++; break;
            case TileType::H : actualTileCounts[7]++; break;
            case TileType::I : actualTileCounts[8]++; break;
            case TileType::J :
                 if(next->getPrey() == PreyType::None)
                 {
                    actualTileCounts[9]++;
                 }
                 else if(next->getPrey() == PreyType::Buffalo)
                 {
                    actualTileCounts[10]++;
                 }
                break;
            case TileType::K :
                if(next->getPrey() == PreyType::None)
                {
                    actualTileCounts[11]++;
                }
                else if(next->getPrey() == PreyType::Boar)
                {
                    actualTileCounts[12]++;
                }
                break;
            case TileType::L :
                if(next->getPrey() == PreyType::None)
                {
                    actualTileCounts[13]++;
                }
                else if(next->getPrey() == PreyType::Boar)
                {
                    actualTileCounts[14]++;
                }
                break;
            case TileType::N : actualTileCounts[15]++; break;
            case TileType::P :
                if(next->getPrey() == PreyType::None)
                {
                    actualTileCounts[16]++;
                }
                else if(next->getPrey() == PreyType::Buffalo)
                {
                    actualTileCounts[17]++;
                }
                break;
            case TileType::R : actualTileCounts[18]++; break;
            case TileType::T :
                if(next->getPrey() == PreyType::None)
                {
                    actualTileCounts[19]++;
                }
                else if(next->getPrey() == PreyType::Croc)
                {
                    actualTileCounts[20]++;
                }
                break;
            case TileType::U : actualTileCounts[21]++; break;
            case TileType::V : actualTileCounts[22]++; break;
            case TileType::W : actualTileCounts[23]++; break;
            case TileType::X : actualTileCounts[24]++; break;
            case TileType::Y : actualTileCounts[25]++; break;
            case TileType::Z :
                if(next->getPrey() == PreyType::None)
                {
                    actualTileCounts[26]++;
                }
                else if(next->getPrey() == PreyType::Deer)
                {
                    actualTileCounts[27]++;
                }
                break;
        }
    }

    for(unsigned int i = 0; i < 28; i++)
    {
        EXPECT_EQ(actualTileCounts[i], expectedTileCounts[i]) << "CreateTiles Index = " << i << std::endl;
    }
}

TEST(BoardManagerTests, getValidMoves)
{

    BoardManager::gameInit();

    unsigned int tileIdCounter = 100;

    TileStack* tileStack = BoardManager::getTileStack();
    unsigned int initalSize = tileStack->getSize();

    Array<Array<Tile>> tiles = Tile::CreateTiles();

    Tile& tile1 = tiles[9][0];//Tile::CreateTileJ(1, tileIdCounter, PreyType::None)[0];
    Coord coord1 = Coord(76, 75);
    const Move& move1 = Move(tile1, coord1, 2, (unsigned int) 1); // rotation = 2, meeple at edge 1

    std::vector<Move> validMoves1 = BoardManager::getValidMoves(tile1, 1);

    EXPECT_EQ(validMoves1.size(), (unsigned int)(6)); 
/*
    std::string expectedValidMoves1[66] = { "PLACE TILE (id) 25 AT 76 75 ROTATION 90",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 90 MEEPLE (edge) 0",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 90 MEEPLE (edge) 1",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 90 MEEPLE (edge) 2",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 90 MEEPLE (edge) 10",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 90 MEEPLE (edge) 12",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 90 MEEPLE (edge) 4",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 90 MEEPLE (edge) 8",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 90 MEEPLE (edge) 7",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 90 MEEPLE (edge) 5",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 90 CROCODILE",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 180",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 180 MEEPLE (edge) 0",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 180 MEEPLE (edge) 1",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 180 MEEPLE (edge) 2",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 180 MEEPLE (edge) 10",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 180 MEEPLE (edge) 12",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 180 MEEPLE (edge) 4",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 180 MEEPLE (edge) 8",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 180 MEEPLE (edge) 7",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 180 MEEPLE (edge) 5",
    "PLACE TILE (id) 25 AT 76 75 ROTATION 180 CROCODILE",
    "PLACE TILE (id) 25 AT 77 76 ROTATION 180",
    "PLACE TILE (id) 25 AT 77 76 ROTATION 180 MEEPLE (edge) 0",
    "PLACE TILE (id) 25 AT 77 76 ROTATION 180 MEEPLE (edge) 1",
    "PLACE TILE (id) 25 AT 77 76 ROTATION 180 MEEPLE (edge) 2",
    "PLACE TILE (id) 25 AT 77 76 ROTATION 180 MEEPLE (edge) 10",
    "PLACE TILE (id) 25 AT 77 76 ROTATION 180 MEEPLE (edge) 12",
    "PLACE TILE (id) 25 AT 77 76 ROTATION 180 MEEPLE (edge) 4",
    "PLACE TILE (id) 25 AT 77 76 ROTATION 180 MEEPLE (edge) 8",
    "PLACE TILE (id) 25 AT 77 76 ROTATION 180 MEEPLE (edge) 7",
    "PLACE TILE (id) 25 AT 77 76 ROTATION 180 MEEPLE (edge) 5",
    "PLACE TILE (id) 25 AT 77 76 ROTATION 180 CROCODILE",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 0",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 0 MEEPLE (edge) 0",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 0 MEEPLE (edge) 1",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 0 MEEPLE (edge) 2",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 0 MEEPLE (edge) 10",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 0 MEEPLE (edge) 12",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 0 MEEPLE (edge) 4",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 0 MEEPLE (edge) 8",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 0 MEEPLE (edge) 7",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 0 MEEPLE (edge) 5",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 0 CROCODILE",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 270",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 270 MEEPLE (edge) 0",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 270 MEEPLE (edge) 1",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 270 MEEPLE (edge) 2",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 270 MEEPLE (edge) 10",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 270 MEEPLE (edge) 12",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 270 MEEPLE (edge) 4",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 270 MEEPLE (edge) 8",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 270 MEEPLE (edge) 7",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 270 MEEPLE (edge) 5",
    "PLACE TILE (id) 25 AT 76 77 ROTATION 270 CROCODILE",
    "PLACE TILE (id) 25 AT 75 76 ROTATION 90",
    "PLACE TILE (id) 25 AT 75 76 ROTATION 90 MEEPLE (edge) 0",
    "PLACE TILE (id) 25 AT 75 76 ROTATION 90 MEEPLE (edge) 1",
    "PLACE TILE (id) 25 AT 75 76 ROTATION 90 MEEPLE (edge) 2",
    "PLACE TILE (id) 25 AT 75 76 ROTATION 90 MEEPLE (edge) 10",
    "PLACE TILE (id) 25 AT 75 76 ROTATION 90 MEEPLE (edge) 12",
    "PLACE TILE (id) 25 AT 75 76 ROTATION 90 MEEPLE (edge) 4",
    "PLACE TILE (id) 25 AT 75 76 ROTATION 90 MEEPLE (edge) 8",
    "PLACE TILE (id) 25 AT 75 76 ROTATION 90 MEEPLE (edge) 7",
    "PLACE TILE (id) 25 AT 75 76 ROTATION 90 MEEPLE (edge) 5",
    "PLACE TILE (id) 25 AT 75 76 ROTATION 90 CROCODILE" };

    for(unsigned int i = 0; i < validMoves1.size(); i++)
    {
        Move move = validMoves1[i];

        std::ostringstream oss;
        oss << "PLACE TILE (id) " << (tile1.getId()) << " AT " << move.getCoord() << " ROTATION " << (move.getRotation() * 90);
        
        if(move.getMeepleLocation() >= 0)
        {
            oss << " MEEPLE (edge) " << move.getMeepleLocation();
        }
        if(move.getHasCrocodile())
        {
            oss << " CROCODILE";
        }

        std::string actualValidMove = oss.str();
        
        EXPECT_EQ(actualValidMove, expectedValidMoves1[i]);
    }*/

    EXPECT_FALSE(tile1.isPlaced());
    BoardManager::makeMove(move1, 1);
    EXPECT_TRUE(tile1.isPlaced());

    Tile& tile2 = tiles[22][0]; // V
    Coord coord2 = Coord(77, 75);
    Move move2 = Move(tile2, coord2, 0, true); // rotation = 0, croc

    std::vector<Move> validMoves2 = BoardManager::getValidMoves(tile2, 2);

    EXPECT_EQ(validMoves2.size(), (unsigned int)8);

    ASSERT_EQ(tile1.getRotation(), 2);

    /*for(unsigned int i = 0; i < validMoves2.size(); i++)
    {
        Move move = validMoves2[i];

        std::ostringstream oss;
        oss << "PLACE TILE (id) " << (tile2.getId()) << " AT " << move.getCoord() << " ROTATION " << (move.getRotation() * 90);
        
        if(move.getMeepleLocation() >= 0)
        {
            oss << " MEEPLE (edge) " << move.getMeepleLocation();
        }
        if(move.getHasCrocodile())
        {
            oss << " CROCODILE";
        }

        std::string actualValidMove = oss.str();
        std::cout << actualValidMove << std::endl;
        //EXPECT_EQ(actualValidMove, expectedValidMoves1[i]);
    }*/

    BoardManager::makeMove(move2, 2);

    Tile& tile3 = tiles[11][0]; // K
    Coord coord3 = Coord(77, 76);
    Move move3 = Move(tile3, coord3, 2, (unsigned int) 10); // rotation = 2, meeple at edge 0

    std::vector<Move> validMoves3 = BoardManager::getValidMoves(tile3, 1);

    EXPECT_EQ(validMoves3.size(), (unsigned int)8);

    BoardManager::makeMove(move3, 1);

    /*for(unsigned int i = 0; i < validMoves3.size(); i++)
    {
        Move move = validMoves3[i];

        std::ostringstream oss;
        oss << "PLACE TILE (id) " << (tile3.getId()) << " AT " << move.getCoord() << " ROTATION " << (move.getRotation() * 90);
        
        if(move.getMeepleLocation() >= 0)
        {
            oss << " MEEPLE (edge) " << move.getMeepleLocation();
        }
        if(move.getHasCrocodile())
        {
            oss << " CROCODILE";
        }

        std::string actualValidMove = oss.str();
        if(move.getMeepleLocation() == -1 && !move.getHasCrocodile())
        {
            std::cout << actualValidMove << std::endl;
        }
        //EXPECT_EQ(actualValidMove, expectedValidMoves1[i]);
    }*/
}

// Should this also test the calls of Regions::addConection,addMeeple,addCroc, as applicable?
TEST(BoardManagerTests, makeMove)
{ 
    BoardManager::gameInit();

    unsigned int tileIdCounter = 100;

    TileStack* tileStack = BoardManager::getTileStack();
    unsigned int initalSize = tileStack->getSize();

    Tile& tile1 = Tile::CreateTileJ(1, tileIdCounter, PreyType::None)[0];
    Coord coord1 = Coord(76, 75);
    const Move& move1 = Move(tile1, coord1, 2);

    EXPECT_EQ(nullptr, Board::get(coord1));
    EXPECT_EQ(tileStack->getSize(), initalSize);
    EXPECT_FALSE(tile1.isPlaced());
    
    BoardManager::makeMove(move1, 1);

    EXPECT_EQ(&tile1, Board::get(coord1));
    EXPECT_EQ(tileStack->getSize(), initalSize - 1);
    EXPECT_TRUE(tile1.isPlaced());

    Tile& tile2 = Tile::CreateTileV(1, tileIdCounter, PreyType::None)[0];
    Coord coord2 = Coord(77, 75);
    const Move& move2 = Move(tile2, coord2);    

    EXPECT_EQ(nullptr, Board::get(coord2));
    EXPECT_EQ(tileStack->getSize(), initalSize - 1);
    EXPECT_FALSE(tile2.isPlaced());

    BoardManager::makeMove(move2, 2);

    EXPECT_EQ(&tile2, Board::get(coord2));
    EXPECT_EQ(tileStack->getSize(), initalSize - 2);
    EXPECT_TRUE(tile2.isPlaced());

    Tile& tile3 = Tile::CreateTileK(1, tileIdCounter, PreyType::None)[0];
    Coord coord3 = Coord(77, 76);
    const Move& move3 = Move(tile3, coord3, 2);

    EXPECT_EQ(nullptr, Board::get(coord3));
    EXPECT_EQ(tileStack->getSize(), initalSize - 2);
    EXPECT_FALSE(tile3.isPlaced());

    BoardManager::makeMove(move3, 1);

    EXPECT_EQ(&tile3, Board::get(coord3));
    EXPECT_EQ(tileStack->getSize(), initalSize - 3);
    EXPECT_TRUE(tile3.isPlaced()); 
}
TEST(BoardManagerTests, isSurrounded)
{
    BoardManager::gameInit();
    Coord center = Coord(76, 76);
    unsigned int centerTileId = Board::get(center)->getId();

    unsigned int tileIdCounter = 100;

    Array<Tile> tiles(8);

    tiles[0] = Tile::CreateTileA(1, tileIdCounter, PreyType::None)[0];
    tiles[1] = Tile::CreateTileB(1, tileIdCounter, PreyType::None)[0];
    tiles[2] = Tile::CreateTileC(1, tileIdCounter, PreyType::None)[0];
    tiles[3] = Tile::CreateTileD(1, tileIdCounter, PreyType::None)[0];
    tiles[4] = Tile::CreateTileE(1, tileIdCounter, PreyType::None)[0];
    tiles[5] = Tile::CreateTileG(1, tileIdCounter, PreyType::None)[0];
    tiles[6] = Tile::CreateTileH(1, tileIdCounter, PreyType::None)[0];
    tiles[7] = Tile::CreateTileI(1, tileIdCounter, PreyType::None)[0];

    int dx[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };
    int dy[8] = { 1, 1, 1, 0, -1, -1, -1, 0 };

    EXPECT_EQ(BoardManager::isSurrounded(centerTileId), (unsigned int)0);

    for(unsigned int i = 0; i < 8; i++)
    {
        Move move = Move(tiles[i], 76 + dx[i], 76 + dy[i]);
        Board::place(move);
        EXPECT_TRUE(BoardManager::getBoard()[76 + dx[i]][76 + dy[i]] == &tiles[i]);
        EXPECT_EQ(BoardManager::isSurrounded(centerTileId), i+1);
    }
}
