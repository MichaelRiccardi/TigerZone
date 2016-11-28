#ifndef __BOARD_MANAGER_H
#define __BOARD_MANAGER_H
#define NUMBER_OF_PLAYERS 2

#include "../Tiles/Tile.h"
#include "../Board/Board.h"
#include "../BoardRules/Rules.h"
#include "Coord.h"
#include "Move.h"
#include <string.h>

#include <vector>

//typedef Array<Tile> (*TileFunction)(unsigned int, unsigned int&, PreyType); // function pointer type
class BoardManager {
    
    public:
        ~BoardManager();
    
        /// Returns the current Board
        static const Array<Array<Tile*>>& getBoard();
        /// Sets up a fresh Board with starting Tile for a new game
        static void gameInit();
        /// Returns all following Tiles to be played
        static TileStack* getTileStack();
        static Tile& getTopTileStack();
        /// Gets all possible legal moves with the given Tile and current Board
        static std::vector<Move> getValidMoves(const Tile& tile);
        static std::vector<Move> getValidMoves(const Tile& tile, unsigned int playerNumber);
        /// Makes the given Move for the given playerNumber
        static void makeMove(const Move& move, unsigned int playerNumber);
		/// Returns whether the given tile is fully surrounded by other tiles
		static unsigned int isSurrounded(int tileID);
        //Entry points for functions needed by the AI
        static struct moveResult tryMove(const Tile& tile, int meepleEdge = -1, bool specialMeeple = false);

        //Handle special cases.
        static void cannotPlaceTile(const Move& move, unsigned int playerNumber);
        static void inputTileStack(char stack[], int length);
        static int setupHashMap();
        static std::unordered_map<std::string, int> getTileFunctionFromName;


    
#ifndef __testing
    private:
#endif

        static Tile * startingTile;
        // remaining Tiles to be played
        static TileStack* tileStack;
        static unsigned int tileIDCounter;
};

#endif
