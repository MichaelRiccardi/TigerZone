#include "BoardManager.h"

BoardManager::BoardManager(TigerZoneGame* game)
{
    this->game = game;
    this->tileStack = new TileStack(NUMBER_OF_PLAYERS);
    this->tileIDCounter = 0;
    this->startingTile = NULL;
    this->getTileFunctionFromName = std::unordered_map<std::string, int>();
    this->setupHashMap();
}

void BoardManager::setupHashMap()
{   

    this->getTileFunctionFromName["JJJJ-"] = 25;    //Tile::CreateTileY;
    this->getTileFunctionFromName["JJJJX"] = 4;     //Tile::CreateTileB;
    this->getTileFunctionFromName["JJTJX"] = 1;     //Tile::CreateTileA;
    this->getTileFunctionFromName["TTTT-"] = 0;     //Tile::CreateTileX;
    this->getTileFunctionFromName["TJTJ-"] = 21;    //Tile::CreateTileU;
    this->getTileFunctionFromName["TJJT-"] = 22;    //Tile::CreateTileV;
    this->getTileFunctionFromName["TJTT-"] = 23;    //Tile::CreateTileW;
    this->getTileFunctionFromName["LLLL-"] = 2;     //Tile::CreateTileC;
    this->getTileFunctionFromName["JLLL-"] = 18;    //Tile::CreateTileR;
    this->getTileFunctionFromName["LLJJ-"] = 15;    //Tile::CreateTileN;
    this->getTileFunctionFromName["JLJL-"] = 6;     //Tile::CreateTileG;
    this->getTileFunctionFromName["LJLJ-"] = 7;     //Tile::CreateTileH;
    this->getTileFunctionFromName["LJJJ-"] = 5;     //Tile::CreateTileE;
    this->getTileFunctionFromName["JLLJ-"] = 8;     //Tile::CreateTileI;
    this->getTileFunctionFromName["TLJT-"] = 11;    //Tile::CreateTileK;
    this->getTileFunctionFromName["TLJTP"] = 12;    //Tile::CreateTileK;
    this->getTileFunctionFromName["JLTT-"] = 9;     //Tile::CreateTileJ;
    this->getTileFunctionFromName["JLTTB"] = 10;    //Tile::CreateTileJ;
    this->getTileFunctionFromName["TLTJ-"] = 3;     //Tile::CreateTileD;
    this->getTileFunctionFromName["TLTJD"] = 4;     //Tile::CreateTileD;
    this->getTileFunctionFromName["TLLL-"] = 19;    //Tile::CreateTileT;
    this->getTileFunctionFromName["TLTT-"] = 13;    //Tile::CreateTileL;
    this->getTileFunctionFromName["TLTTP"] = 14;    //Tile::CreateTileL;
    this->getTileFunctionFromName["TLLT-"] = 16;    //Tile::CreateTileP;
    this->getTileFunctionFromName["TLLTB"] = 17;    //Tile::CreateTileP;
    this->getTileFunctionFromName["LJTJ-"] = 26;    //Tile::CreateTileZ;
    this->getTileFunctionFromName["LJTJD"] = 27;    //Tile::CreateTileZ;
    this->getTileFunctionFromName["TLLLC"] = 20;    //Tile::CreateTileZ;
}

BoardManager::~BoardManager()
{
    if (startingTile != NULL) delete startingTile;
    if (tileStack != NULL) delete tileStack;
}

const Array<Array<Tile*>>& BoardManager::getBoard()
{
    return game->board->getBoard();
}

void BoardManager::gameInit()
{
     // Set/reset the Board
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
                game->board->place(startingMove);
                const Tile ** borderingTiles = game->board->getBorderingTiles(*startingTile);
                game->regions->addConnection(*startingTile, borderingTiles);
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
    std::unordered_set<unsigned int> availableLocations = game->board->getAvailableLocations();
    const unsigned int daveTigerOrder[9] = { 0, 1, 2, 10, 12, 4, 8, 7, 5 };

    for(const int gridId : availableLocations)
    {
        const Coord location = game->board->getCoordinatesFromGridId(gridId);
        const Tile ** borderingTiles = game->board->getBorderingTiles(location);
        Tile tileCopy = tile;

        for(unsigned int rotation = 0; rotation < (unsigned int) NUM_TILE_SIDES; rotation++)
        {
            tileCopy.setRotation(rotation);

            if(game->rules->validTilePlacement(tileCopy, borderingTiles))
            {
                validMoves.push_back(Move(tileCopy, Coord(location), rotation)); // no meeple or croc

                Array<bool> validMeeplePlacements = game->rules->validMeeplePlacement(tileCopy, location);

                for(unsigned int daveTigerIndex = 0; daveTigerIndex < 9; daveTigerIndex++)
                {
                    if(validMeeplePlacements[daveTigerIndex])//game->rules->validMeeplePlacement(location, edgeIndex))
                    {
                        validMoves.push_back(Move(tileCopy, Coord(location), rotation, (unsigned int) daveTigerOrder[daveTigerIndex]));
                    }
                }

                if(game->rules->validCrocPlacement(tile, location))
                {
                    validMoves.push_back(Move(tileCopy, Coord(location), rotation, true));
                }
            }   
        }
    }

    if(validMoves.size() == 0) // special cases woo
    {
        validMoves.push_back(Move(false)); // pass
        for(unsigned int i = 7 * (playerNumber - 1); i < (7 * playerNumber); i++)
        {
            if(game->regions->ownerMeepleInUse(i))
            {
                unsigned int meepleTileId = game->regions->getMeepleTileId(i);
                Tile& meepleTile = *game->board->get(meepleTileId);
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
    game->board->place(move);

    Tile& tile = move.getTile();
    tile.setRotation(move.getRotation());

    const Tile ** borderingTiles = game->board->getBorderingTiles(tile);
    game->regions->addConnection(tile, borderingTiles);    

    if(move.getMeepleLocation() != -1) // if Move includes Meeple
    {
        game->regions->addMeeple(playerNumber, tile.getId(), move.getMeepleLocation());
    }
    else if(move.getHasCrocodile())
    {
        game->regions->addCroc(playerNumber, tile.getId());
    }

    tile.placeTile(); // mark Tile as placed so it can no longer be rotated
    tileStack->pop(); // remove top Tile from listg
}


void BoardManager::cannotPlaceTile(const Move& move, unsigned int playerNumber)
{
    //Code special cases.
    if(!move.getPickupMeeple()) // if Move includes Meeple
    {
        game->regions->addMeepleSpecial(playerNumber, move.getTile().getId());
    }
    else 
    {
        game->regions->specialRemoveMeeple(playerNumber, move.getTile().getId());
    }
}

unsigned int BoardManager::isSurrounded(int tileID)
{
    unsigned int surrounded = 0;
    //printf("[surrounded] tileID %d\n", tileID);
    const Coord& coord = game->board->getCoordinatesFromTileId(tileID);
    const Array<Array<Tile*>>& boardGrid = game->board->getBoard();
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
    return game->regions->tryMove(tile, game->board->getBorderingTiles(tile), meepleEdge, specialMeeple);
}


void BoardManager::inputTileStack(char stack[], int length)
{
    /*
    if(sizeof(*stack) != (length * 5 + 1))
    {
        throw std::logic_error("sizeof stack and anticipated stack size differ");
    }
    */
    //printf("Why\n");
    //printf("Why not\n");
    Array<Array<Tile>> tiles = Tile::CreateTiles();
    //printf("Hoopla\n");

    std::string currentTile;
    int offset;
    for(int i = (length*5) - 5; i >= 0; i -= 5)          //Skip over the null char and the first set of chars.
    {
        //printf("Trying to assign\n");
        currentTile.assign(stack + i, stack + i + 5);
        //printf("Tile stack index : %d name %s\n", i, currentTile.c_str());

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
