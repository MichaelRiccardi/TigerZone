#include "BoardManager.h"

TileStack* BoardManager::tileStack = new TileStack(NUMBER_OF_PLAYERS);
unsigned int BoardManager::tileIDCounter = 0;
Tile * BoardManager::startingTile = NULL;
std::unordered_map<std::string, int> BoardManager::getTileFunctionFromName = std::unordered_map<std::string, int>();
int v = BoardManager::setupHashMap();


int BoardManager::setupHashMap()
{
    Board::set();                                           // Set/reset the Board
    tileStack = new TileStack(NUMBER_OF_PLAYERS);           // Initialize Tile stack

    BoardManager::getTileFunctionFromName["JJJJ-"] = 25;    //Tile::CreateTileY;
    BoardManager::getTileFunctionFromName["JJJJX"] = 4;     //Tile::CreateTileB;
    BoardManager::getTileFunctionFromName["JJTJX"] = 1;     //Tile::CreateTileA;
    BoardManager::getTileFunctionFromName["TTTT-"] = 0;     //Tile::CreateTileX;
    BoardManager::getTileFunctionFromName["TJTJ-"] = 21;    //Tile::CreateTileU;
    BoardManager::getTileFunctionFromName["TJJT-"] = 22;    //Tile::CreateTileV;
    BoardManager::getTileFunctionFromName["TJTT-"] = 23;    //Tile::CreateTileW;
    BoardManager::getTileFunctionFromName["LLLL-"] = 2;     //Tile::CreateTileC;
    BoardManager::getTileFunctionFromName["JLLL-"] = 18;    //Tile::CreateTileR;
    BoardManager::getTileFunctionFromName["LLJJ-"] = 15;    //Tile::CreateTileN;
    BoardManager::getTileFunctionFromName["JLJL-"] = 6;     //Tile::CreateTileG;
    BoardManager::getTileFunctionFromName["LJLJ-"] = 7;     //Tile::CreateTileH;
    BoardManager::getTileFunctionFromName["LJJJ-"] = 5;     //Tile::CreateTileE;
    BoardManager::getTileFunctionFromName["JLLJ-"] = 8;     //Tile::CreateTileI;
    BoardManager::getTileFunctionFromName["TLJT-"] = 11;    //Tile::CreateTileK;
    BoardManager::getTileFunctionFromName["TLJTP"] = 12;    //Tile::CreateTileK;
    BoardManager::getTileFunctionFromName["JLTT-"] = 9;     //Tile::CreateTileJ;
    BoardManager::getTileFunctionFromName["JLTTB"] = 10;    //Tile::CreateTileJ;
    BoardManager::getTileFunctionFromName["TLTJ-"] = 3;     //Tile::CreateTileD;
    BoardManager::getTileFunctionFromName["TLTJD"] = 4;     //Tile::CreateTileD;
    BoardManager::getTileFunctionFromName["TLLL-"] = 19;    //Tile::CreateTileT;
    BoardManager::getTileFunctionFromName["TLTT-"] = 13;    //Tile::CreateTileL;
    BoardManager::getTileFunctionFromName["TLTTP"] = 14;    //Tile::CreateTileL;
    BoardManager::getTileFunctionFromName["TLLT-"] = 16;    //Tile::CreateTileP;
    BoardManager::getTileFunctionFromName["TLLTB"] = 17;    //Tile::CreateTileP;
    BoardManager::getTileFunctionFromName["LJTJ-"] = 26;    //Tile::CreateTileZ;
    BoardManager::getTileFunctionFromName["LJTJD"] = 27;    //Tile::CreateTileZ;
    BoardManager::getTileFunctionFromName["TLLLC"] = 20;    //Tile::CreateTileZ;
    return 0;
}






BoardManager::~BoardManager()
{
    if (startingTile != NULL) delete startingTile;
    if (tileStack != NULL) delete tileStack;
}

const Array<Array<Tile*>>& BoardManager::getBoard()
{
    return Board::getBoard();
}

void BoardManager::gameInit()
{
    Board::set(); // Set/reset the Board
    tileStack = new TileStack(NUMBER_OF_PLAYERS); // Initialize Tile stack

    // Build the Tile list
    Array<Array<Tile>> tiles = Tile::CreateTiles();

    std::vector<Tile> tileList;

    for(unsigned int i = 0; i < tiles.getSize(); i++)
    {
        for(unsigned int j = 0; j < tiles[i].getSize(); j++)
        {
            if(tiles[i][j].getTileType() == TileType::D && tiles[i][j].getPrey() == PreyType::None && j == 0)
            {
                // Place starting tile in center
                Coord center(NUMBER_OF_PLAYABLE_TILES, NUMBER_OF_PLAYABLE_TILES);
                if (startingTile != NULL) delete startingTile;
                startingTile = new Tile(tiles[i][j]);
                Move startingMove(*startingTile, center);
                Board::place(startingMove);
                const Tile ** borderingTiles = Board::getBorderingTiles(*startingTile);
                Regions::addConnection(*startingTile, borderingTiles);
                startingTile->placeTile();
            }
            else
            {
                tileList.push_back(tiles[i][j]);
            }
        }
    }

    // Randomize tile order
    std::random_shuffle( tileList.begin(), tileList.end() );

    // Build the TileStack from randomized list
    for(unsigned int i = 0; i < tileList.size(); i++)
    {
        tileStack->push(tileList[i]);
    }
}

TileStack* BoardManager::getTileStack()
{
    return tileStack;
}

Tile& BoardManager::getTopTileStack()
{
    return tileStack->front();
}

std::vector<Move> BoardManager::getValidMoves(const Tile& tile, unsigned int playerNumber)
{
    std::vector<Move> validMoves;
    std::unordered_set<unsigned int> availableLocations = Board::getAvailableLocations();
    //unsigned int daveTigerOrder[9] = { 0, 1, 2, 10, 12, 4, 8, 7, 5 };

    for(const int gridId : availableLocations)
    {
        const Coord location = Board::getCoordinatesFromGridId(gridId);
        const Tile ** borderingTiles = Board::getBorderingTiles(location);
        Tile tileCopy = tile;

        for(unsigned int rotation = 0; rotation < (unsigned int) NUM_TILE_SIDES; rotation++)
        {
            tileCopy.setRotation(rotation);

            if(GameRules::validTilePlacement(tileCopy, borderingTiles))
            {
                validMoves.push_back(Move(tileCopy, Coord(location), rotation)); // no meeple or croc

                /*for(unsigned int daveTigerIndex = 0; daveTigerIndex < 9; daveTigerIndex++)
                {
                    unsigned int edgeIndex = daveTigerOrder[daveTigerIndex];

                    if(GameRules::validMeeplePlacement(location, edgeIndex))
                    {
                        validMoves.push_back(Move(tileCopy, Coord(location), rotation, (unsigned int) edgeIndex));
                    }
                }

                if(GameRules::validCrocPlacement(tile, location))
                {
                    validMoves.push_back(Move(tileCopy, Coord(location), rotation, true));
                }*/
            }   
        }
    }

    if(validMoves.size() == 0) // special cases woo
    {
        validMoves.push_back(Move(false)); // pass
        for(unsigned int i = 7 * (playerNumber - 1); i < (7 * playerNumber); i++)
        {
            if(Regions::ownerMeepleInUse(i))
            {
                unsigned int meepleTileId = Regions::getMeepleTileId(i);
                Tile& meepleTile = *Board::get(meepleTileId);
                validMoves.push_back(Move(meepleTile, true)); // place another
                validMoves.push_back(Move(meepleTile, false)); // remove
            }
        }
    }

    return validMoves;
}

void BoardManager::makeMove(const Move& move, unsigned int playerNumber)
{
    // if calling this method, it is assumed that this is a legal move
    Board::place(move);

    Tile& tile = move.getTile();
    tile.setRotation(move.getRotation());

    const Tile ** borderingTiles = Board::getBorderingTiles(tile);
    //Regions::addConnection(tile, borderingTiles);

    /*if(move.getMeepleLocation() != -1) // if Move includes Meeple
    {
        Regions::addMeeple(playerNumber, tile.getId(), move.getMeepleLocation());
    }
    else if(move.getHasCrocodile())
    {
        Regions::addCroc(playerNumber, tile.getId());
    }*/

    tile.placeTile(); // mark Tile as placed so it can no longer be rotated
    tileStack->pop(); // remove top Tile from list
}


void BoardManager::cannotPlaceTile(const Move& move, unsigned int playerNumber)
{
    //Code special cases.
    if(!move.getPickupMeeple()) // if Move includes Meeple
    {
        Regions::addMeepleSpecial(playerNumber, move.getTile().getId());
    }
    else 
    {
        Regions::specialRemoveMeeple(playerNumber, move.getTile().getId());
    }
}

unsigned int BoardManager::isSurrounded(int tileID)
{
    unsigned int surrounded = 0;
    const Coord& coord = Board::getCoordinatesFromTileId(tileID);
    const Array<Array<Tile*>>& boardGrid = Board::getBoard();
    unsigned int xLocation = coord.getX();
    unsigned int yLocation = coord.getY();

    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {
            int thisX = xLocation + i;
            int thisY = yLocation + j;
            if (thisX >= 0 && thisY >= 0 && thisX < (int)boardGrid.getSize() && thisY < (int)boardGrid[0].getSize() &&
                boardGrid[xLocation + i][yLocation + j] != NULL && !(i == 0 && j == 0))
            {
                surrounded++;
            }
        }
    }
    return surrounded;
}

//Entry points for AI
struct moveResult BoardManager::tryMove(const Tile& tile, int meepleEdge, bool specialMeeple)
{
    return Regions::tryMove(tile, Board::getBorderingTiles(tile), meepleEdge, specialMeeple);
}


void BoardManager::inputTileStack(char stack[], int length)
{
    /*
    if(sizeof(*stack) != (length * 5 + 1))
    {
        throw std::logic_error("sizeof stack and anticipated stack size differ");
    }
    */
    
    Array<Array<Tile>> tiles = Tile::CreateTiles();

    std::string currentTile;
    int offset;
    for(int i = length - 6; i > 0; i -= 5)          //Skip over the null char and the first set of chars.
    {
            
        currentTile.assign(stack + i, stack + i + 5);

        auto iter = getTileFunctionFromName.find(currentTile);
        if (iter != getTileFunctionFromName.end())
        {
            offset = tileStack->getCount(tiles[(iter->second)][0].getTileType(), tiles[(iter->second)][0].getPrey());
            tileStack->push(tiles[(iter->second)][offset]);
        }
        else
        {
            throw std::logic_error("Could not find the index for tile.");
        }
    }
}
