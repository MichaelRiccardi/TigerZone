#ifndef __RULES_H
#define __RULES_H

#include "../Board/Board.h"
#include "../BoardManager/BoardManager.h"
#include "../Tiles/Tile.h"
#include "Regions.h"

#include <unordered_map>

#define ROAD_VALUE              1
#define CASTLE_VALUE            2
#define CHURCH_VALUE            9
#define FIELD_CASTLE_VALUE      3
#define FIELD_CHURCH_VALUE      5



class GameRules
{
    public:
        GameRules();
        static bool validTilePlacement(const Tile& placed, const Tile ** boarderingTiles);
        static bool validMeeplePlacement(const Tile& placed, unsigned int edgeIndex);
        static unsigned int getCurrentScore(unsigned int tileID, unsigned int edge);
        static unsigned int getCurrentScore(struct regionSet ** currentRegion, unsigned int edge, const Tile * tile, bool isSurrounded);
        static unsigned int scoreEdge(unsigned int tileID, unsigned int edge);
        static unsigned int getPlayerScore(unsigned int player);
    private:
        static unsigned int scoreRoad(struct regionSet * currentSet, bool actuallyScore);
        static unsigned int scoreCastle(struct regionSet * currentSet, bool actuallyScore);
        static unsigned int scoreGrass(struct regionSet ** currentSets, unsigned int tileID, unsigned int edge);
        static unsigned int scoreChurch(unsigned int isSurrounded, bool actuallyScore);
        static unsigned int player1Score;
        static unsigned int player2Score;
>>>>>>> 3689a625f862d967931a8c312230916050328e97
};



#endif
