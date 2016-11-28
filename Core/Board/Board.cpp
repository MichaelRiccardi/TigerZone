#include "Board.h"

std::unordered_map<unsigned int, Move> Board::tileIDTracker = std::unordered_map<unsigned int, Move>();
std::unordered_set<unsigned int> Board::availableLocations = std::unordered_set<unsigned int>();
TileStack tileStack(unsigned int NUMBER_OF_PLAYERS);

const unsigned int Board::boardWidth = NUMBER_OF_PLAYABLE_TILES * 2 + 1;
const unsigned int Board::boardHeight = NUMBER_OF_PLAYABLE_TILES * 2 + 1;
Array<Array<Tile*>> Board::board = Array<Array<Tile*>>(boardWidth);
//int throwaway = Board::set();

int Board::set()
{
    Board::availableLocations.clear();
    Board::tileIDTracker.clear();

    for(unsigned int i = 0; i < Board::boardWidth; i++)
    {
        Board::board[i] = Array<Tile*>(boardHeight);
        for(unsigned int j = 0; j < Board::boardHeight; j++)
        {
            Board::board[i][j] = nullptr;
        }
    }

    return 1;
}

const Array<Array<Tile*>>& Board::getBoard()
{
    return Board::board;
}

Tile* Board::get(const Coord& coord)
{
    return board[coord.getX()][coord.getY()];
}

Tile* Board::get(unsigned int tileID)
{
    auto tileSearch = tileIDTracker.find(tileID);
    if(tileSearch != tileIDTracker.end())
    {
        return Board::get((tileSearch->second).getCoord());
    }
    else
    {
        return nullptr;
    }
}

const Tile** Board::getBorderingTiles(const Coord& coord)
{
    const unsigned int numberOfBorderingTiles = 8;
    const Tile** borderingTiles = new const Tile*[numberOfBorderingTiles];

    // not exactly sure why this order makes sense
    int dx[numberOfBorderingTiles] = { 1, 0, -1, -1, -1, 0, 1, 1 }; 
    int dy[numberOfBorderingTiles] = { -1, -1, -1, 0, 1, 1, 1, 0 };

    const Array<Array<Tile*>>& boardGrid = Board::getBoard();

    for(unsigned int i = 0; i < numberOfBorderingTiles; i++)
    {
        unsigned int x = coord.getX() + dx[i];
        unsigned int y = coord.getY() + dy[i];

        if(x < 0 || x >= boardWidth || y < 0 || y >= boardWidth) // out of range
        {
            borderingTiles[i] = nullptr;
            continue;
        }

        borderingTiles[i] = boardGrid[x][y];
    }

    return borderingTiles;
}

const Tile** Board::getBorderingTiles(const Tile& tile)
{
    const Coord& coord = Board::getCoordinatesFromTileId(tile.getId());
    return Board::getBorderingTiles(coord);
}

const Coord Board::getCoordinatesFromGridId(unsigned int gridId)
{
    const Coord coord(gridId % boardWidth, gridId / boardWidth);
    return coord;
}

unsigned int Board::getGridId(const Coord& coord)
{
    unsigned int gridId = coord.getY() * boardWidth + coord.getX();
    return gridId;
}

unsigned int Board::getGridId(unsigned int x, unsigned int y)
{
    unsigned int gridId = y * boardWidth + x;
    return gridId;
}

void Board::place(const Move& move)
{
    const Coord& coord = move.getCoord();
    Tile& tile = move.getTile();
    unsigned int gridId = Board::getGridId(coord);

    availableLocations.erase(gridId);
    int dx[4] = { -1, 1, 0, 0 };
    int dy[4] = { 0, 0, -1, 1 };

    const Array<Array<Tile*>>& boardGrid = Board::getBoard();

    for(unsigned int i = 0; i < 4; i++)
    {
        unsigned int x = coord.getX() + dx[i];
        unsigned int y = coord.getY() + dy[i];

        if(x < 0 || x >= boardWidth || y < 0 || y >= boardWidth) // out of range
        {
            continue;
        }

        if(boardGrid[x][y] == NULL)
        {
            availableLocations.insert(Board::getGridId(x, y));
        }
    }

    board[coord.getX()][coord.getY()] = &tile;

    //Used for accounting
    //Move& moveCopy = move; // not const
    tileIDTracker.insert( std::pair<unsigned int, Move>(tile.getId(), move) );
}

const std::unordered_set<unsigned int>& Board::getAvailableLocations()
{
    return availableLocations;
}

const Coord& Board::getCoordinatesFromTileId(unsigned int tileID)
{
    auto tileSearch = tileIDTracker.find(tileID);
    if(tileSearch != tileIDTracker.end())
    {
        return tileSearch->second.getCoord();
    }
    else
    {
        throw std::invalid_argument("Invalid Tile ID");
    }
}
