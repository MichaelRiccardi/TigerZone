#include "Regions.h"

void Regions::mergeRegions(int placedTileID, int placedEdge, int connectingTileID, int connectingEdge)
{
    auto placedSearch = regionTracker.find(placedTileID);
    auto connectingSearch = regionTracker.find(connectingTileID);
    if(placedSearch != regionTracker.end() && connectingSearch != regionTracker.end())
    {
        //Update meeple values
        (connectingSearch->second[connectingEdge])->player1Meeples      += (placedSearch->second[placedEdge])->player1Meeples;
        (connectingSearch->second[connectingEdge])->player2Meeples      += (placedSearch->second[placedEdge])->player2Meeples;

        //Update edgesTillCompletion, no need to increment becuase addConnection() handles
        (connectingSearch->second[connectingEdge])->edgesTillCompletion += (placedSearch->second[placedEdge])->edgesTillCompletion;

        //Take over the linked list.
        ((connectingSearch->second[connectingEdge])->tail)->next         = (placedSearch->second[placedEdge])->head;
        (connectingSearch->second[connectingEdge])->tail                 = (placedSearch->second[placedEdge])->tail;

        //Update Hash entries
        struct tileNode *iter = (placedSearch->second[placedEdge])->head;
        while(iter != NULL)
        {
            regionTracker[(placedSearch->first)] = (connectingSearch->second[connectingEdge]);
            iter = iter->next;
        }

        //Get rid of old region.
        delete placedSearch->second[connectingEdge];
    }
}

int Regions::addConnection(const Tile& newTile, const Tile ** boarderingTiles) {
    unsigned int numOfSides = newTile.getNumberOfSides();
    unsigned int countPerSide = newTile.getCountPerSide();
    unsigned int totalEdges = numOfSides * countPerSide;
    unsigned int id = newTile.getId();

    struct regionSet * newRegions = new struct regionSet*[totalEdges];
    regionTracker.at(id) = newRegions;

    for (unsigned int edge = 0; edge < totalEdges; edge++) {
        unsigned int side = edge / countPerSide;
        if (boardingTiles[side] == NULL) {
            newRegions[edge] = NULL;
            continue;
        }

        unsigned int correspondingSide = (((edge / countPerSide) + (numOfSides / 2)) % numOfSides;
        unsigned int correspondingEdge = (countPerSide - (edge % countPerSide) - 1) + countPerSide * correspondingSide;

        unsigned int boarderingId = boarderingTiles[side]->getId();

        newRegions[edge] = regionTracker.at(boarderingId)[correspondingEdge];
    }

    for (unsigned int edge = 0; edge < totalEdges; edge++) {
        if (newRegions[edge] != NULL) continue;

        newRegions[edge] = createRegion(id, edge);

        for (unsigned int otherEdge = edge + 1; otherEdge < totalEdges; otherEdge++) {
           if (!newTile.isConnected(edge, otherEdge)) continue;
        }
    }
}

int Regions::addConnection(int placedTileID, int placedEdge, int connectingTileID, int connectingEdge)
{
    if(connectingTileID == -1) //Empty Edge
    {
        createRegion(placedTileID, placedEdge);
        auto search = regionTracker.find(tileID);
        if(search != regionTracker.end())
        {
          //
        }
    }
    auto search = regionTracker.find(tileID);
    if(search != example.end())
    {
        (search->second[placedEdge])->edgesTillCompletion--;
        //Add connection logic
        //Loop through tile sides to find connections
        (search->second[placedEdge])->edgesTillCompletion++;
        return 0;
    }

    return -1;
}

int Regions::addMeeple(int playerNumber, int tileID, int edge)
{
    int i;
    bool valid = false;
    for(i = (playerNumber)*7; i < ((playerNumber)*7 + 7); i++)
    {
        if(!(ownerMeeples[i].inUse))
        {
            valid = true;
        }
    }
    if(!valid)
    {
        return -1;
    }

    if(Regions::checkOwner(tileID, edge) == -2) //No owner
    {
        ownerMeeples[i].inUse = true;
        ownedMeeples[i].ownedRegion = regionTracker.find(tileID)[edge];
        return 0;
    }
    return -1;
}

int Regions::checkOwner(int tileID, int edge)
{
    auto search = regionTracker.find(tileID, edge);
    if(search != example.end())
    {
        if((search->second[edge])->player1Meeples > (search->second[edge])->player2Meeples)
        {
            return 1;
        }
        else if((search->second[edge])->player1Meeples == (search->second[edge])->player2Meeples)
        {
            return 0;
        }
        return -1;
    }
    return -2;
}

bool Regions::validTilePlacement(const Tile& placed, const Tile * boarderingTiles, int arraySize) {
    unsigned int sides = placed.getNumberOfSides();
    unsigned int countPerSide = placed.getCountPerSide();
    unsigned int edgeCount = sides * countPerSide;

    for (unsigned int edge = 0; edge < edgeCount; edge++) {
        unsigned int correspondingSide = (((edge / countPerSide) + (sides / 2)) % sides;
        unsigned int correspondingEdge = (countPerSide - (edge % countPerSide) - 1) + countPerSide * correspondingSide;

        if (placed.getTerrainType(edge) != boarderingTiles[correspondingSide].getTerrainType(correspondingEdge)) return false;
    }

    return true;
}

bool Regions::validMeeplePlacement(const Tile& placed, unsigned int edgeIndex) {
    unsigned int id = placed.getId();
    bool hasRegion = regionTracker.count(id) > 0;
    if (!hasRegion) return true;

    bool hasPlayer1 = regionTracker.at(id)[edgeIndex]->player1Meeples > 0;
    bool hasPlayer2 = regionTracker.at(id)[edgeIndex]->player2Meeples > 0;

    return ((!hasPlayer1) && (!hasPlayer2));
}
