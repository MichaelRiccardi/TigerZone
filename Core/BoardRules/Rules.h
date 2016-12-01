#ifndef __RULES_H
#define __RULES_H

#include "../Board/Board.h"
#include "../BoardManager/BoardManager.h"
#include "../Tiles/Tile.h"
#include "Regions.h"

#include <unordered_map>
#include <memory>

#define ROAD_VALUE              1
#define CASTLE_VALUE            2
#define CHURCH_VALUE            9
#define FIELD_CASTLE_VALUE      3
#define FIELD_CHURCH_VALUE      5



class GameRules
{
    public:
         GameRules(TigerZoneGame* game);
         bool validTilePlacement(const Tile& placed, const Tile ** boarderingTiles);
         bool validMeeplePlacement(const Tile& placed, unsigned int edgeIndex);
         Array<bool> validMeeplePlacement(const Tile& toBePlaced, const Coord& location);
         bool validCrocPlacement(unsigned int tileID);
         bool hasCroc(unsigned int tileID);
         bool checkSideForCroc(unsigned int x, unsigned int y);
         bool validCrocPlacement(const Tile& toPlace, const Coord& location);
         unsigned int getCurrentScore(unsigned int tileID, unsigned int edge);
         unsigned int getCurrentScore(std::shared_ptr<struct regionSet> * currentRegions, unsigned int edge, const Tile * tile, unsigned int tilesSurrounded);
         unsigned int scoreEdge(unsigned int tileID, unsigned int edge, bool endOfGame = false);
         unsigned int getPlayerScore(unsigned int player);
#ifndef __testing
    private:
#endif
         TigerZoneGame* game;
         unsigned int scoreRoad(std::shared_ptr<struct regionSet> currentSet, bool actuallyScore);
         unsigned int scoreCastle(std::shared_ptr<struct regionSet> currentSet, bool actuallyScore, bool endOfGame = false);
         unsigned int scoreGrass(std::shared_ptr<struct regionSet> * currentSets, unsigned int tileID, unsigned int edge, const Tile * passedTile = NULL);
         unsigned int scoreChurch(unsigned int isSurrounded, bool actuallyScore);
         unsigned int player1Score;
         unsigned int player2Score;
};



#endif
