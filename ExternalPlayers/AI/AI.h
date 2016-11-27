#ifndef __AI_H
#define __AI_H


#include "FuzzyLogic.h"
#include "../../Core/BoardManager/BoardManager.h"
#include "../../Core/BoardRules/Regions.h"
#include "../../Core/Board/Board.h"

class AI
{
public:
    static Move& chooseTurn(Tile& currentTile);
    static void setPlayerNumber(unsigned int playerNumber);
private:
    static FuzzyLogic fz;
    static AIMove move;
    static unsigned int myPlayerNumber;
};

#endif
