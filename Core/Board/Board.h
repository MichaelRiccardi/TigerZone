#ifndef __BOARD_H
#define __BOARD_H
#define NUMBER_OF_PLAYABLE_TILES 76
#define NUMBER_OF_PLAYERS 2

#include "../../Common/Array.h"
#include "../Tiles/Tile.h"
#include "../Tiles/TileStack.h"
#include "../BoardManager/Coord.h"
#include "../BoardManager/Move.h"
#include <unordered_set>
#include <stdexcept>

class TigerZoneGame;

class Board {
    public:
        // Initializes or resets the Board to a blank/empty grid
        Board(TigerZoneGame* game);
        // Returns the Board as a 2D Array of Tiles
        const Array<Array<Tile*>>& getBoard();
        // Gets the Tile at the given Coord
        Tile* get(const Coord& coord);
        //Gets the Tile for a given tileID
        Tile* get(unsigned int tileID);
        //Get all neighbors of a given Coord or place Tile
        const Tile** getBorderingTiles(const Coord& coord);
        const Tile** getBorderingTiles(const Tile& tile);
        //Find a way to get the coordinates from the ID
        const Coord& getCoordinatesFromTileId(unsigned int tileID);
        // GridId is used for the set of Available Locations
        const Coord getCoordinatesFromGridId(unsigned int gridId);
        // Get the GridId for a given Coord
        unsigned int getGridId(const Coord& coord);
        unsigned int getGridId(unsigned int x, unsigned int y);
        // For a given Move, place the specifed Tile at the specified Coord
        void place(const Move& move);
        // Returns a set of gridID's of loactions adjacent to the border of already placed tiles
        const std::unordered_set<unsigned int>& getAvailableLocations();

#ifndef __testing
    private:
#endif
        TigerZoneGame* game;
        Array<Array<Tile*>> board;
        TileStack* tileStack;
        //Keep track of the tile coordinates;
        std::unordered_map<unsigned int , Move> tileIDTracker;
        std::unordered_set<unsigned int> availableLocations;
        unsigned int boardWidth;
        unsigned int boardHeight;
};

#endif
