#ifndef __TIGERZONE_GAME_H
#define __TIGERZONE_GAME_H

#include "../Core/BoardManager/Move.h"
#include "../Core/BoardManager/Coord.h"
#include "../Core/Tiles/Tile.h"
#include "../Core/Board/Board.h"
#include "../Core/BoardRules/Regions.h"
#include "../Core/BoardRules/Rules.h"
#include "../ExternalPlayers/AI/AI.h"
#include <unordered_map>
#include <memory>

class Board;
class BoardManager;
class Regions;
class GameRules;
class AI;

class TigerZoneGame
{
    public:
         TigerZoneGame();
         Board* board;
         BoardManager* boardManager;
         Regions* regions;
         GameRules* rules;
         AI* ai;

    private:

};

#endif
