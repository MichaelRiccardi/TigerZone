#include "Rules.h"



GameRules::GameRules()
{
    GameRules::player1Score = 0;
    GameRules::player1Score = 0;
}

bool GameRules::validTilePlacement(const Tile& placed, const Tile ** boarderingTiles)
{
    unsigned int sides = placed.getNumberOfSides();
    unsigned int countPerSide = placed.getCountPerSide();
    unsigned int edgeCount = sides * countPerSide;

    for (unsigned int edge = 0; edge < edgeCount; edge++) {
        unsigned int correspondingSide = ((edge / countPerSide) + (sides / 2)) % sides;
        unsigned int correspondingEdge = (countPerSide - (edge % countPerSide) - 1) + countPerSide * correspondingSide;

        if (placed.getTerrainType(edge) != boarderingTiles[correspondingSide]->getTerrainType(correspondingEdge)) return false;
    }

    return true;
}

bool GameRules::validMeeplePlacement(const Tile& placed, unsigned int edgeIndex)
{
    unsigned int id = placed.getId();
    bool hasRegion = Regions::getRegions(id) != NULL;
    if (!hasRegion) return true;

    bool hasPlayer1 = Regions::getRegions(id)[edgeIndex]->player1Meeples > 0;
    bool hasPlayer2 = Regions::getRegions(id)[edgeIndex]->player2Meeples > 0;

    return ((!hasPlayer1) && (!hasPlayer2));
}


unsigned int GameRules::scoreRoad(struct regionSet * currentSet, bool actuallyScore)
{
    std::unordered_map<unsigned int, bool> edgeTracker;
    //Init starting values
    struct tileNode * currentNode = currentSet->head;    
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
            preyCount = 0;
            //Enter the tileID to make sure we don't revisit it
            edgeTracker[currentNode->tileID] = 1;
            for(int i = 0; i  < NUM_PREY; i++)
            {
                //Scoring for animal-road interaction
                preyCount += currentNode->preyValues[i];
            }
            score += ROAD_VALUE + preyCount;
        }
        //Get the next node in the list
        currentNode = currentNode->next;
    }
    return score;
}

unsigned int GameRules::scoreCastle(struct regionSet * currentSet, bool actuallyScore)
{
    std::unordered_map<unsigned int, bool> edgeTracker;
    //Init starting values
    struct tileNode * currentNode = currentSet->head;    
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
            preyCount = 0;
            //Make sure we don't revisit the tileID
            edgeTracker[currentNode->tileID] = 1;
            for(int i = 0; i  < NUM_PREY; i++)
            {
                if(currentNode->preyValues[i])
                {
                    preyCount++;
                }
            }
            //Scoring for castle-animal interaction
            score += CASTLE_VALUE * (1 + preyCount);
        }
        //Get the next node in the list
        currentNode = currentNode->next;
    }
    return score;
}

unsigned int GameRules::scoreGrass(unsigned int tileID, unsigned int edge)
{
    unsigned int score = 0;
    unsigned int leftOfEdge, rightOfEdge;
    struct regionSet ** currentSets = Regions::getRegions(tileID);
    std::unordered_map<struct regionSet * , bool> fieldTracker;
    //Init starting values
    struct tileNode * currentNode = (currentSets[edge])->head;    
    auto tileSearch = fieldTracker.find(currentSets[edge]);
    const Tile * currentTile;

    //Iterate through the linked list of the given region
    while(currentNode != NULL)
    {        
        //Get all of the regions for the current tileID (associated with the current node)
        currentSets = Regions::getRegions(currentNode->tileID);
        //We need the actual tile to be able to determine which regions are actually touching.
        currentTile = Board::get(tileID);

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
}

unsigned int GameRules::scoreChurch(bool isSurrounded)
{
    unsigned int score = 0;

    if(isSurrounded)
    {
        score += 9;
    }

    return score;
}


//Entry point for scoring a region.
unsigned int GameRules::getCurrentScore(unsigned int tileID, unsigned int edge)
{
    bool isSurrounded = BoardManager::isSurrounded(tileID);

    struct regionSet ** currentRegion = Regions::getRegions(tileID);
    unsigned int returnValue = 0;

    switch (currentRegion[edge]->type)
    {
        case TerrainType::Grass:
            returnValue = scoreGrass(tileID, edge);
            break;
        case TerrainType::Road:
            returnValue = scoreRoad(currentRegion[edge], false);
            break;
        case TerrainType::Castle:
            returnValue = scoreCastle(currentRegion[edge], false);
            break;
        case TerrainType::Church:
            returnValue = scoreChurch(isSurrounded);
            break;
        default:
            //Throw error
            break;
    }
    return returnValue;
}


unsigned int GameRules::scoreEdge(unsigned int tileID, unsigned int edge)
{
    bool isSurrounded = BoardManager::isSurrounded(tileID);

    struct regionSet ** currentRegion = Regions::getRegions(tileID);
    unsigned int returnValue = 0;

    switch (currentRegion[edge]->type)
    {
        case TerrainType::Grass:
            returnValue = scoreGrass(tileID, edge);
            break;
        case TerrainType::Road:
            returnValue = scoreRoad(currentRegion[edge], true);
            break;
        case TerrainType::Castle:
            returnValue = scoreCastle(currentRegion[edge], true);
            break;
        case TerrainType::Church:
            returnValue = scoreChurch(isSurrounded);
            break;
        default:
            //Throw error
            break;
    }

    if (returnValue != 0)
    {
        Regions::removeMeeple(tileID, edge);
    }

    //Update the current score for the owning player(s)
    int ret = Regions::checkOwner(tileID, edge);
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