#include "TigerZoneGame.h"

TigerZoneGame::TigerZoneGame()
{
    this->board = new Board(this);
    this->boardManager = new BoardManager(this);
    this->regions = new Regions(this);
    this->rules = new GameRules(this);
    this->ai = new AI(this);
}