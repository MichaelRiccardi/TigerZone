#ifndef __AI_H
#define __AI_H

#include <typeinfo>
#include "FuzzyLogic.h"
#include "../../Core/BoardManager/BoardManager.h"
#include "../../Core/BoardRules/Regions.h"
#include "../../Core/Board/Board.h"

class AI
{
public:
	AI(TigerZoneGame* game);
    Move chooseTurn(const Tile& currentTile);
    void setPlayerNumber(unsigned int playerNumber);
    std::vector<Move> getMoveList();
#ifndef __testing
private:
#endif
	TigerZoneGame* game;
    FuzzyLogic fz;
    AIMove move;
    unsigned int myPlayerNumber;
};

#endif
