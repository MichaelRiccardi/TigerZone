#include "AI.h"

AI::AI(TigerZoneGame* game)
{
    this->game = game;
}

#ifndef __testing
Move AI::chooseTurn(const Tile& currentTile)
{
    std::vector<Move> moveList = game->boardManager->getValidMoves((Tile&)currentTile, this->myPlayerNumber);

    auto highestIndex = moveList.begin();
    float highestValue = 0;

    float currentValue;
    for(auto currentMove = moveList.begin(); currentMove != moveList.end(); ++currentMove)
    {
        struct moveResult currentResult = game->boardManager->tryMove(currentMove->getTile());

        if (this->myPlayerNumber == 1)
        {
            this->move.edgesTillCompletion = currentResult.edgesTillCompletion;
            this->move.diffMyScore = currentResult.player1ScoreChange;
            this->move.diffEnemyScore = currentResult.player2ScoreChange;
        }
        else
        {
            this->move.edgesTillCompletion = currentResult.edgesTillCompletion;
            this->move.diffEnemyScore = currentResult.player1ScoreChange;
            this->move.diffMyScore = currentResult.player2ScoreChange;
        }

        this->fz.enterData(this->move);
        currentValue = this->fz.getResults();

        if(currentValue > highestValue)
        {
            highestValue = currentValue;
            highestIndex = currentMove;
        }
    }
    return moveList.at(highestIndex - moveList.begin()) ;
}
#else
/*
struct moveResult
{
    unsigned int edgesTillCompletion;
    int player1ScoreChange;
    int player2ScoreChange;
};
*/
struct moveResult pseudoTryMove(int i)
{
    struct moveResult ret;
    ret.edgesTillCompletion = 10 - i;
    ret.player1ScoreChange = i;
    ret.player2ScoreChange = 10 - i;
    //printf("Randomized values: edgesTillCompletion %d player1ScoreChange %d player2ScoreChange %d\n", ret.edgesTillCompletion, ret.player1ScoreChange, ret.player2ScoreChange);
    return ret;
}

std::vector<Move> AI::getMoveList()
{
    std::vector<Move> mvs;
    for(int i = 0; i < 10; i++)
    {
        Coord c = Coord(i, 2);
        //printf("Coord : %d %d\n", c.getX(), c.getY());
        Move m = Move(game->boardManager->getTopTileStack(), c);
        mvs.push_back(m);
    }
    return mvs;
}


Move AI::chooseTurn(const Tile& currentTile)
{
    std::vector<Move> moveList = this->getMoveList();
    auto highestIndex = moveList.begin();
    float highestValue = 0;

    float currentValue;
    int i = 0;
    for(auto currentMove = moveList.begin(); currentMove != moveList.end(); ++currentMove)
    {
        struct moveResult currentResult = pseudoTryMove(i);

        if (this->myPlayerNumber == 1)
        {
            this->move.edgesTillCompletion = currentResult.edgesTillCompletion;
            this->move.diffMyScore = currentResult.player1ScoreChange;
            this->move.diffEnemyScore = currentResult.player2ScoreChange;
        }
        else
        {
            this->move.edgesTillCompletion = currentResult.edgesTillCompletion;
            this->move.diffEnemyScore = currentResult.player1ScoreChange;
            this->move.diffMyScore = currentResult.player2ScoreChange;
        }

        this->fz.enterData(this->move);
        currentValue = this->fz.getResults();

        //printf("Coord : %d %d\nscore :%4.4f\n", currentMove->getCoord().getX(), currentMove->getCoord().getY(), currentValue);

        if(currentValue > highestValue)
        {
            highestValue = currentValue;
            highestIndex = currentMove;
        }
        i++;
    }
    //printf("movelist.at() %d\n", (moveList.at(highestIndex - moveList.begin())).getCoord().getX());
    //std::cout << typeid(moveList.at(highestIndex - moveList.begin())).name() << '\n';

    Move ret = moveList.at(highestIndex - moveList.begin());
    return ret;
}
#endif


void AI::setPlayerNumber(unsigned int playerNumber)
{
    this->myPlayerNumber = playerNumber;
}
