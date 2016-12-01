#include "Rules.h"

GameRules::GameRules(TigerZoneGame* game)
{
    this->game = game;
    this->player1Score = 0;
    this->player2Score = 0;
}

bool GameRules::validTilePlacement(const Tile& placed, const Tile ** boarderingTiles)
{
    unsigned int sides = placed.getNumberOfSides();
    unsigned int countPerSide = placed.getCountPerSide();
    unsigned int edgeCount = sides * countPerSide;

    if (boarderingTiles == NULL) return true;

    for (unsigned int edge = 0; edge < edgeCount; edge++) {
        unsigned int correspondingSide = ((edge / countPerSide) + (sides / 2)) % sides;
        unsigned int correspondingEdge = (countPerSide - (edge % countPerSide) - 1) + countPerSide * correspondingSide;
        unsigned int boarderingTile = ((2 * correspondingSide) + 5) % 8;

        if (boarderingTiles[boarderingTile] == NULL) continue;
        if (placed.getTerrainType(edge) != boarderingTiles[boarderingTile]->getTerrainType(correspondingEdge)) return false;
    }

    return true;
}

bool GameRules::validMeeplePlacement(const Tile& placed, unsigned int edgeIndex)
{
    unsigned int id = placed.getId();
    std::shared_ptr<struct regionSet> * regions = game->regions->getRegions(id);
    if (regions == NULL) return true;

    bool hasPlayer1 = regions[edgeIndex]->player1Meeples > 0;
    bool hasPlayer2 = regions[edgeIndex]->player2Meeples > 0;

    return ((!hasPlayer1) && (!hasPlayer2));
}
#import <iostream>
Array<bool> GameRules::validMeeplePlacement(const Tile& toBePlaced, const Coord& location)
{
    const unsigned int daveTigerOrder[9] = { 0, 1, 2, 10, 12, 4, 8, 7, 5 };

    Array<bool> isInvalid((unsigned int) 9);
    for(unsigned int i = 0; i < 9; i++) { isInvalid[i] = false; }

    Array<bool> canPlaceMeeple((unsigned int) 9);
    for(unsigned int i = 0; i < 9; i++) { canPlaceMeeple[i] = false; }

    for(unsigned int t = 0; t < 9; t++)
    {
        if(isInvalid[t])
        {
            continue;
        }

        unsigned int edgeIndex = daveTigerOrder[t];

        unsigned int side = edgeIndex / NUM_TILE_EDGES_PER_SIDE;
        unsigned int correspondingSide = (side + (NUM_TILE_SIDES / 2)) % NUM_TILE_SIDES;
        unsigned int correspondingEdge = (NUM_TILE_EDGES_PER_SIDE - (edgeIndex % NUM_TILE_EDGES_PER_SIDE) - 1) + (NUM_TILE_EDGES_PER_SIDE * correspondingSide);
        unsigned int newX = location.getX();
        unsigned int newY = location.getY();

        if (side == 0) newY += 1;
        else if (side == 1) newX += 1;
        else if (side == 2) newY += -1;
        else if (side == 3) newX += -1;

        Coord neighbor = Coord(newX, newY);

        const Tile* neighborTile = game->board->get(neighbor);

        if (neighborTile == NULL)
        {
            canPlaceMeeple[t] = true;
            continue;
        }

        std::shared_ptr<struct regionSet> * regions = game->regions->getRegions(neighborTile->getId());
        if (regions == NULL)
        {
            canPlaceMeeple[t] = true;
            continue;
        }

        //std::cout << "DAVE TIGER ORDER IS FUN " << edgeIndex << std::endl;

        for(unsigned int i = 0; i < 12; i++)
        {
            if(regions[i]->player1Meeples > 0)
            {
                //std::cout << regions[i]->player1Meeples << " meeples for regions[" << i << "]->player1Meeples" << std::endl;
            }
            if(regions[i]->player2Meeples > 0)
            {
                //std::cout << regions[i]->player2Meeples << " meeples for regions[" << i << "]->player2Meeples" << std::endl;
            }
        }

        //std::cout << location << " with " << edgeIndex << " neighbor (" << newX << ", " << newY << ")" << std::endl;
        //std::cout << game->regions->checkOwner(neighborTile->getId(), correspondingEdge) << std::endl;

        canPlaceMeeple[t] = (game->regions->checkOwner(neighborTile->getId(), correspondingEdge) == -2);

        if(!canPlaceMeeple[t])
        {
            for(unsigned int tt = 0; tt < 9; tt++)
            {
                if(daveTigerOrder[tt] == edgeIndex) continue;
                if(toBePlaced.isConnected(edgeIndex, daveTigerOrder[tt]))
                {
                    isInvalid[tt] = true;
                    canPlaceMeeple[tt] = false;
                }
            }
        }
    }

    //std::cout << std::endl;
    
    return canPlaceMeeple;
}

bool GameRules::hasCroc(unsigned int tileID)
{
    std::shared_ptr<struct regionSet> * regions = game->regions->getRegions(tileID);

    if(regions == nullptr)
    {
        return false;
    }

    for(int i = 0; i < NUM_TILE_EDGES + 1; i++)
    {
        if(regions[i] != NULL && regions[i]->hasCroc)
        {
            return true;
        }
    }
    return false;
}

bool GameRules::validCrocPlacement(unsigned int tileID)
{
    const Tile *currentTile = game->board->get(tileID);
    bool valid = false;
    for(int i = 0; i < NUM_TILE_EDGES + 1; i++)
    {
        if(currentTile->getTerrainType(i) == TerrainType::Road || currentTile->getTerrainType(i) == TerrainType::Castle)
        {
            valid = true;
            break;
        }
    }

    if(!valid)
    {
        return false;
    }

    return !(GameRules::hasCroc(tileID));
}


bool GameRules::checkSideForCroc(unsigned int x, unsigned int y)
{
    Coord sideCoord = Coord(x,y);
    Tile* sideTile = game->board->get(sideCoord);
    if(sideTile == nullptr) return false;
    unsigned int tileID = sideTile->getId();
    return hasCroc(tileID);  //If the adjacent tile regions return valid move, no croc.
}

bool GameRules::validCrocPlacement(const Tile& toPlace, const Coord& location)
{
    int currentX = location.getX();
    int currentY = location.getY();

    bool valid = false;
    for(int i = 0; i < NUM_TILE_EDGES + 1; i++)
    {
        if(toPlace.getTerrainType(i) == TerrainType::Road || toPlace.getTerrainType(i) == TerrainType::Castle)
        {
            valid = true;
            break;
        }
    }

    if(!valid)
    {
        return false;
    }

    if(checkSideForCroc(currentX - 1, currentY)) //Croc exists
    {
        return false;
    }

    else if(checkSideForCroc(currentX + 1, currentY)) //Croc exists
    {
        return false;
    }

    else if(checkSideForCroc(currentX, currentY - 1)) //Croc exists
    {
        return false;
    }

    else if(checkSideForCroc(currentX, currentY + 1)) //Croc exists
    {
        return false;
    }

    return true;
}

unsigned int GameRules::scoreRoad(std::shared_ptr<struct regionSet> currentSet, bool actuallyScore)
{
    std::unordered_map<unsigned int, bool> edgeTracker;
    //Init starting values
    std::shared_ptr<struct tileNode> currentNode = currentSet->head;
    auto tileSearch = edgeTracker.find(currentNode->tileID);
    unsigned int score = 0;
    unsigned int preyCount = 0;

    if(actuallyScore && currentSet->edgesTillCompletion != 0)
    {
        return 0;
    }

    //Iterate through the linked list of the region
    while(currentNode != NULL)
    {
        //Search for an entry in the map
        tileSearch = edgeTracker.find(currentNode->tileID);
        //If the entry doesn't exist, we haven't visited the tile yet
        if(tileSearch == edgeTracker.end())
        {
            //printf("tile %d score before %d\n", currentNode->tileID, score);
            preyCount = 0;
            //Enter the tileID to make sure we don't revisit it
            edgeTracker[currentNode->tileID] = 1;
            for(int i = 0; i  < NUM_PREY; i++)
            {
                if(i == 3 && currentNode->preyCounts[i]) //Croc index
                {
                    preyCount--; //Eats prey.
                    continue;
                }
                //Scoring for animal-road interaction
                preyCount += currentNode->preyCounts[i];
            }
            score += ROAD_VALUE + preyCount;
            //printf("after score %d\n", score);
        }
        //Get the next node in the list
        currentNode = currentNode->next;
    }
    return score;
}

unsigned int GameRules::scoreCastle(std::shared_ptr<struct regionSet> currentSet, bool actuallyScore, bool endOfGame)
{
    std::unordered_map<unsigned int, bool> edgeTracker;
    //Init starting values
    std::shared_ptr<struct tileNode> currentNode = currentSet->head;
    auto tileSearch = edgeTracker.find(currentNode->tileID);
    unsigned int score = 0;
    unsigned int preyCount = 0;

    if(actuallyScore && currentSet->edgesTillCompletion != 0 && !endOfGame)
    {
        return 0;
    }


    //Iterate through the linked list of the region
    while(currentNode != NULL && currentNode->tileID != -1)
    {
        //Search for an entry in the map
        tileSearch = edgeTracker.find(currentNode->tileID);
        //If the entry doesn't exist, we haven't visited the tile yet
        if(tileSearch == edgeTracker.end())
        {
            //printf("before score %d\n", score);
            preyCount = 0;
            //Make sure we don't revisit the tileID
            edgeTracker[currentNode->tileID] = 1;
            for(int i = 0; i  < NUM_PREY; i++)
            {
                //printf("tile preyCount value %d at %d\n", currentNode->preyCounts[i], i);
                if(i == 3 && currentNode->preyCounts[i])
                {
                    preyCount--; //eaten by a croc
                    continue;
                }
                if(currentNode->preyCounts[i] > 0)
                {
                    preyCount++;
                }
            }
            //Scoring for castle-animal interaction
            if(!endOfGame) {score += CASTLE_VALUE * (1 + preyCount);}
            if(endOfGame) {score += 1 * (1 + preyCount);}
            //printf("tileID %d preyCount %d score %d\n", currentNode->tileID, preyCount, score);
        }
        //Get the next node in the list
        currentNode = currentNode->next;
    }
    return score;
}

unsigned int GameRules::scoreGrass(std::shared_ptr<struct regionSet> * passedSets, unsigned int tileID, unsigned int edge, const Tile * passedTile)
{
    //printf("enter\n");
    unsigned int score = 0;
    unsigned int leftOfEdge, rightOfEdge;
    std::unordered_map<std::shared_ptr<struct regionSet>, bool> fieldTracker;
    //Init starting values
    std::shared_ptr<struct tileNode> currentNode = (passedSets[edge])->head;
    auto tileSearch = fieldTracker.find(passedSets[edge]);
    std::shared_ptr<struct regionSet> * currentSets = passedSets;

                        return 0;
/*
    //Iterate through the linked list of the given region
    while(currentNode != NULL)
    {
        printf("tile %d has score %d", currentNode->tileID, score);
        if ((unsigned int)currentNode->tileID == tileID) {
            currentSets = passedSets;
            currentTile = passedTile;
        }
        else {
            //Get all of the regions for the current tileID (associated with the current node)
            currentSets = game->regions->getRegions(currentNode->tileID);
            //We need the actual tile to be able to determine which regions are actually touching.
            currentTile = game->board->get(tileID);
        }

        //Init the starting values of left and right.
        if(currentNode->edge == (NUM_TILE_EDGES - 1)) rightOfEdge = 0;
        else rightOfEdge = currentNode->edge + 1;
        if(currentNode->edge == 0) leftOfEdge = (NUM_TILE_EDGES - 1);
        else leftOfEdge = currentNode->edge - 1;


        //Check through all edges on tile to the left of the edge inside of the region we care about
        for(; leftOfEdge != edge; leftOfEdge--)
        {
            if(!(currentTile->isConnected(leftOfEdge, edge)))
            {
                break; //Found the adjacent region
            }
            if(leftOfEdge == 0)
            {
                leftOfEdge = NUM_TILE_EDGES; //Decrement will make this the right number
            }
        }

        //Check through all edges on tile to the right of the edge inside of the region we care about
        for(; rightOfEdge != edge; rightOfEdge--)
        {
            if(!(currentTile->isConnected(rightOfEdge, edge)))
            {
                break; //Found the adjacent region
            }
            if(rightOfEdge == (NUM_TILE_EDGES - 1))
            {
                rightOfEdge = 0; //Decrement will make this the right number
            }
        }

        tileSearch = fieldTracker.find(currentSets[leftOfEdge]);
        //If we haven't scored this region before, and leftOfEdge hasn't looped back around
        if(leftOfEdge != edge && tileSearch == fieldTracker.end())
        {
            if((currentSets[leftOfEdge]->type == TerrainType::Church) && (currentSets[leftOfEdge]->edgesTillCompletion == 0))
            {
                //Score it, and add it to the hash map
                score += FIELD_CHURCH_VALUE;
                //Add the region to the hash map to make sure we don't re-score it.
                fieldTracker[currentSets[leftOfEdge]] = true;
            }
            else if((currentSets[leftOfEdge]->type == TerrainType::Castle) && (currentSets[leftOfEdge]->edgesTillCompletion == 0))
            {
                //Score it, and add it to the hash map
                score += FIELD_CASTLE_VALUE;
                //Add the region to the hash map to make sure we don't re-score it.
                fieldTracker[currentSets[leftOfEdge]] = true;
            }

        }

        tileSearch = fieldTracker.find(currentSets[rightOfEdge]);
        if(rightOfEdge != edge && tileSearch == fieldTracker.end())
        {
            if((currentSets[rightOfEdge]->type == TerrainType::Church) && (currentSets[rightOfEdge]->edgesTillCompletion == 0))
            {
                //Score it, and add it to the hash map
                score += FIELD_CHURCH_VALUE;
                //Add the region to the hash map to make sure we don't re-score it.
                fieldTracker[currentSets[rightOfEdge]] = true;
            }
            else if((currentSets[rightOfEdge]->type == TerrainType::Castle) && (currentSets[rightOfEdge]->edgesTillCompletion == 0))
            {
                //Score it, and add it to the hash map
                score += FIELD_CASTLE_VALUE;
                //Add the region to the hash map to make sure we don't re-score it.
                fieldTracker[currentSets[rightOfEdge]] = true;
            }
        }
        currentNode = currentNode->next;
    }
    return score;
    */
}

unsigned int GameRules::scoreChurch(unsigned int tilesSurrounded, bool actuallyScore)
{
    unsigned int score = 0;

    if(tilesSurrounded == 8)
    {
        score += 9;
    }
    else if (!actuallyScore)
    {
        score = 1;
        score += tilesSurrounded;
    }

    return score;
}

unsigned int GameRules::getCurrentScore(std::shared_ptr<struct regionSet> * currentRegion, unsigned int edge, const Tile * tile, unsigned int tilesSurrounded)
{
    unsigned int returnValue = 0;

    switch (currentRegion[edge]->type)
    {
        case TerrainType::Grass:
            returnValue = scoreGrass(currentRegion, tile->getId(), edge, tile);
            break;
        case TerrainType::Road:
            returnValue = scoreRoad(currentRegion[edge], false);
            break;
        case TerrainType::Castle:
            returnValue = scoreCastle(currentRegion[edge], false);
            break;
        case TerrainType::Church:
            returnValue = scoreChurch(tilesSurrounded, false);
            break;
        default:
            //Throw error
            break;
    }
    return returnValue;
}

//Entry point for scoring a region.
unsigned int GameRules::getCurrentScore(unsigned int tileID, unsigned int edge)
{
    unsigned int tilesSurrounded = game->boardManager->isSurrounded(tileID);

    std::shared_ptr<struct regionSet> * currentRegion = game->regions->getRegions(tileID);
    unsigned int returnValue = 0;

    switch (currentRegion[edge]->type)
    {
        case TerrainType::Grass:
            returnValue = scoreGrass(currentRegion, tileID, edge);
            break;
        case TerrainType::Road:
            returnValue = scoreRoad(currentRegion[edge], false);
            break;
        case TerrainType::Castle:
            returnValue = scoreCastle(currentRegion[edge], false);
            break;
        case TerrainType::Church:
            returnValue = scoreChurch(tilesSurrounded, false);
            break;
        default:
            //Throw error
            break;
    }
    return returnValue;
}


unsigned int GameRules::scoreEdge(unsigned int tileID, unsigned int edge, bool endOfGame)
{
    unsigned int tilesSurrounded = game->boardManager->isSurrounded(tileID);

    std::shared_ptr<struct regionSet> * currentRegion = game->regions->getRegions(tileID);
    unsigned int returnValue = 0;

    switch (currentRegion[edge]->type)
    {
        case TerrainType::Grass:
            returnValue = scoreGrass(currentRegion, tileID, edge);
            break;
        case TerrainType::Road:
            returnValue = scoreRoad(currentRegion[edge], true);
            break;
        case TerrainType::Castle:
            returnValue = scoreCastle(currentRegion[edge], true, endOfGame);
            break;
        case TerrainType::Church:
            returnValue = scoreChurch(tilesSurrounded, true);
            break;
        default:
            //Throw error
            break;
    }

    //Update the current score for the owning player(s)
    int ret = game->regions->checkOwner(tileID, edge);
    if(ret == OWNER_P1)
    {
        GameRules::player1Score += returnValue;
    }
    else if(ret == OWNER_P2)
    {
        GameRules::player2Score += returnValue;
    }
    else if(ret == OWNER_TIE)
    {

        GameRules::player1Score += returnValue;
        GameRules::player2Score += returnValue;
    }


    if (returnValue != 0)
    {
        game->regions->removeMeeple(tileID, edge);
    }

    return returnValue;
}

unsigned int GameRules::getPlayerScore(unsigned int player)
{
    if (player == 1)
    {
        return GameRules::player1Score;
    }
    else if (player == 2)
    {
        return GameRules::player2Score;
    }
    return 0;
}
