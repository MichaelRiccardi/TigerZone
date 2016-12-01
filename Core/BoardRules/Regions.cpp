#include "Regions.h"

Regions::Regions(TigerZoneGame* game)
{
    this->game = game;
    //this->ownerMeeples = {};
    //this->ownerCrocs = {};
    this->regionTracker = std::unordered_map<unsigned int, std::shared_ptr<struct regionSet> *>();
    this->availableMeeples[0] = MEEPLES_PER_PLAYER;
    this->availableMeeples[1] = MEEPLES_PER_PLAYER;
    this->availableCrocs[0] = CROCS_PER_PLAYER;
    this->availableCrocs[1] = CROCS_PER_PLAYER;

}

/***************** Temporary! *****************/
#include <iostream>

std::ostream& operator<<(std::ostream& os, TerrainType t)
{
    switch(t)
    {
        case TerrainType::Grass: os << "Grass";    break;
        case TerrainType::Road: os << "Road"; break;
        case TerrainType::Castle: os << "Castle";  break;
        case TerrainType::Church: os << "Church";   break;
        case TerrainType::Fork: os << "Fork";   break;
        default: os << "Invalid TerrainType";
    }

    return os;
}
/*************** End Temporary! ***************/

int Regions::addCroc(unsigned int playerNumber, unsigned int tileID)
{
    unsigned int i;
    bool valid = false;
    for(i = (playerNumber -  1)*(CROCS_PER_PLAYER); i < ((playerNumber - 1)*(CROCS_PER_PLAYER) + (CROCS_PER_PLAYER)); i++)
    {
        if(!(ownerCrocs[i].inUse))
        {
            valid = true;
// -- logically we should break and use this croc index i? --
            break;
// ----------------------------------------------------------
        }
    }
    if(!valid)
    {
        return -1;
    }

    if(game->rules->validCrocPlacement(tileID))
    {
        ownerCrocs[i].inUse = true;
        ownerCrocs[i].ownedRegions = (regionTracker.find(tileID))->second;

        for(int j = 0; j < NUM_TILE_EDGES; j++)
        {
            ownerCrocs[i].ownedRegions[j]->hasCroc = true;
        }
        this->availableCrocs[playerNumber - 1]--;

        return 0;
    }

    return -1;
}

void Regions::mergeRegions(unsigned int placedTileID, unsigned int placedEdge, unsigned int connectingTileID, unsigned int connectingEdge)
{
    auto placedSearch = regionTracker.find(placedTileID);
    auto connectingSearch = regionTracker.find(connectingTileID);

    if (placedSearch == regionTracker.end() || connectingSearch == regionTracker.end())
    {
        throw std::logic_error("The placed or connecting tile can't be found in the regions.");
    }

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

        //Debug check to make sure we can iterate through all elements in the list
        /*std::shared_ptr<struct tileNode> tmpIter = (connectingSearch->second[connectingEdge])->head;
        while(tmpIter != NULL)
        {
            //std::cout <<"Tile Id: " << tmpIter->tileID << " Edge: " << tmpIter->edge << " at "  << tmpIter << std::endl;
            tmpIter = tmpIter->next;
        }
        //std::cout << "End!" << std::endl << std::endl;
        */
        //Update Hash entries
        std::shared_ptr<struct tileNode> iter = (placedSearch->second[placedEdge])->head;
        while(iter != NULL)
        {
            regionTracker[iter->tileID][iter->edge] = (connectingSearch->second[connectingEdge]);

            iter = iter->next;
        }
    }
}

std::shared_ptr<struct regionSet> * Regions::addConnection(const Tile& newTile, const Tile ** allBoarderingTiles, std::unordered_map<unsigned int, std::shared_ptr<struct regionSet> *> * trackerToUse) {
    //std::cout << "Add Connection Tile: " << newTile.getId() << std::endl;
    unsigned int numOfSides = newTile.getNumberOfSides();
    unsigned int countPerSide = newTile.getCountPerSide();
    unsigned int totalEdges = numOfSides * countPerSide;
    unsigned int id = newTile.getId();
    unsigned int centerEdge = countPerSide / 2;
    std::unordered_map<unsigned int, std::shared_ptr<struct regionSet> *> * tracker = &regionTracker;

    if (trackerToUse != NULL) tracker = trackerToUse;

    // add one to total edges so that we have an array location for the center
    std::shared_ptr<struct regionSet> * newRegions = new std::shared_ptr<struct regionSet>[totalEdges + 1];
    (*tracker)[id] = newRegions;

    if (newTile.getCenter() == TerrainType::Church) {
        newRegions[totalEdges] = this->createRegion(newTile, totalEdges, newTile.getCenter());
        newRegions[totalEdges]->edgesTillCompletion = 8;
    }
    else {
        newRegions[totalEdges] = NULL;
    }

    const Tile* boarderingTiles[NUM_TILE_SIDES];
    unsigned int sideInc = 0;
    for (unsigned int currSide = 0; currSide < numOfSides * 2; currSide++) {
        if (allBoarderingTiles[currSide] != NULL) {
            if (allBoarderingTiles[currSide]->getCenter() == TerrainType::Church) {
                unsigned int boarderingId = allBoarderingTiles[currSide]->getId();

                // decrement the church's region edges till completion tracker if we were placed as a tile around it
                (*tracker)[boarderingId][totalEdges]->edgesTillCompletion--;
            }
            if (newTile.getCenter() == TerrainType::Church) (*tracker)[id][totalEdges]->edgesTillCompletion--;
        }

        if ((currSide % 2) == 1) {
            boarderingTiles[sideInc] = allBoarderingTiles[currSide];
            sideInc++;
        }
    }

    // from what I can tell, this can't be optimised much. This is currently O(n + n^2) and the best case is we get O(n^2)
    for (unsigned int edge = 0; edge < totalEdges; edge++) {
        unsigned int side = edge / countPerSide;
        unsigned int correspondingSide = (side + (numOfSides / 2)) % numOfSides;
        unsigned int correspondingEdge = (countPerSide - (edge % countPerSide) - 1) + (countPerSide * correspondingSide);

        if (boarderingTiles[side] != NULL) {
            unsigned int boarderingId = boarderingTiles[side]->getId();

            //printf("Found tileID : %d at side %d\n", boarderingTiles[side]->getId(), edge);

            newRegions[edge] = (*tracker)[boarderingId][correspondingEdge];

            if (edge % countPerSide == centerEdge)
                newRegions[edge]->edgesTillCompletion--;

            std::shared_ptr<struct tileNode> node = std::shared_ptr<struct tileNode>(new struct tileNode());
            node->tileID = id;
            node->edge = edge;
            node->preyCounts[static_cast<int>(newTile.getPrey())]++;
            node->previous = newRegions[edge]->tail;
            newRegions[edge]->tail->next = node;
            newRegions[edge]->tail = node;
        }
        else {
            newRegions[edge] = NULL;
        }
    }

    for (unsigned int edge = 0; edge < totalEdges; edge++) {
        if (newRegions[edge] == NULL) {
            newRegions[edge] = createRegion(newTile, edge, newTile.getTerrainType(edge));

            //printf("[NULL] Tile %d edge %d edgesTillCompletion %d\n", id, edge, newRegions[edge]->edgesTillCompletion);
            //std::cout << "[NULL] Tile : " << newRegions[ed]

            newRegions[edge]->tail->preyCounts[static_cast<int>(newTile.getPrey())]++;
        }

        for (unsigned int otherEdge = edge + 1; otherEdge < totalEdges; otherEdge++) {
            if (!newTile.isConnected(edge, otherEdge)) continue;
            // what about if we connect a region back to itself (completing a square road)?
            else if (newRegions[otherEdge] == newRegions[edge]) continue;
            else if (newRegions[otherEdge] == NULL) {
                //std::cout << "Extend Region with Edge: " << edge << " with Edge: " << otherEdge << std::endl;
                //std::cout << "Edge " << edge << " Type: " << newTile.getTerrainType(edge) << ". Edge " << otherEdge << " Type: " << newTile.getTerrainType(otherEdge) << std::endl << std::endl;
                unsigned int side = edge / countPerSide;

                newRegions[otherEdge] = newRegions[edge];

                if (otherEdge % countPerSide == centerEdge && boarderingTiles[side] != NULL)
                    newRegions[otherEdge]->edgesTillCompletion++;

                std::shared_ptr<struct tileNode> node = std::shared_ptr<struct tileNode>(new struct tileNode());
                node->tileID = id;
                node->edge = otherEdge;
                node->preyCounts[static_cast<int>(newTile.getPrey())]++;
                node->previous = newRegions[otherEdge]->tail;

                newRegions[otherEdge]->tail->next = node;
                newRegions[otherEdge]->tail = node;
            }
            else {
                //std::cout << "Merge Edge: " << edge << " with Edge: " << otherEdge << std::endl;
                //std::cout << "Edge " << edge << " Type: " << (newTile.getTerrainType(edge)) << ". Edge " << otherEdge << " Type: " << (newTile.getTerrainType(otherEdge)) << std::endl << std::endl;
                mergeRegions(id, edge, id, otherEdge);
            }
        }
    }

    //for (unsigned int edge = 0; edge < totalEdges; edge++) std::cout << "Edges Till Completion for Tile " << id << " and Edge " << edge << ": " << (*tracker)[id][edge]->edgesTillCompletion << std::endl;

    return newRegions;
}

int Regions::addMeeple(unsigned int playerNumber, unsigned int tileID, unsigned int edge, std::unordered_map<unsigned int, std::shared_ptr<struct regionSet> *> * tracker)
{
    if (tracker == NULL) tracker = &regionTracker;

    unsigned int i;
    bool valid = false;
    for(i = (playerNumber -  1)*(MEEPLES_PER_PLAYER); i < ((playerNumber - 1)*(MEEPLES_PER_PLAYER) + (MEEPLES_PER_PLAYER)); i++)
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

    if(this->checkOwner(tileID, edge) == -2) //No owner
    {
        ownerMeeples[i].tileID = tileID;
        ownerMeeples[i].inUse = true;
        ownerMeeples[i].ownedRegion = tracker->find(tileID)->second[edge];
        if(playerNumber == 1)
        {
            ownerMeeples[i].ownedRegion->player1Meeples++;
        }
        else if(playerNumber == 2)
        {
            ownerMeeples[i].ownedRegion->player2Meeples++;
        }
        else
        {
            throw std::invalid_argument(playerNumber + " is not a valid playerNumber");
        }

        this->availableMeeples[playerNumber - 1]--;
        return 0;
    }
    //std::cout << "this->checkOwner(" << tileID << ", " << edge << ") = " << this->checkOwner(tileID, edge) << std::endl;
    return -1;
}

int Regions::addMeepleSpecial(unsigned int playerNumber, unsigned int tileID)
{
    unsigned int i,freeMeeple;
    bool valid = false;

    for(i = (playerNumber -  1)*(MEEPLES_PER_PLAYER); i < ((playerNumber - 1)*(MEEPLES_PER_PLAYER) + (MEEPLES_PER_PLAYER)); i++)
    {
        if(!(ownerMeeples[i].inUse))    // available Meeple
        {
            valid = true;
            freeMeeple = i;
            break;
        }
    }
    if(!valid)
    {
        return -1;  // Nooooo Meeple available
    }

    for(i = (playerNumber -  1)*(MEEPLES_PER_PLAYER); i < ((playerNumber - 1)*(MEEPLES_PER_PLAYER) + (MEEPLES_PER_PLAYER)); i++)
    {
        if((ownerMeeples[i].inUse) && (ownerMeeples[i].tileID == tileID))    // Meeple on tileID
        {
            ownerMeeples[freeMeeple].tileID = tileID;
            ownerMeeples[freeMeeple].inUse = true;
            ownerMeeples[freeMeeple].ownedRegion = ownerMeeples[i].ownedRegion;
            this->availableMeeples[playerNumber - 1]--;
            return 0; // success
        }
    }
    throw std::logic_error("No meeple of player exist on the requested tileID");
    return -1;  //Nooo existing meeple on tile
}

int Regions::removeMeeple(unsigned int tileID, unsigned int edge, std::unordered_map<unsigned int, std::shared_ptr<struct regionSet> *> * tracker)
{
    if (tracker == NULL) tracker = &regionTracker;

    std::shared_ptr<struct regionSet> wantedRegion = tracker->find(tileID)->second[edge];
    for(int i = 0; i < TOTAL_MEEPLES; i++)
    {
        if(ownerMeeples[i].inUse && (ownerMeeples[i].ownedRegion == wantedRegion))
        {
            ownerMeeples[i].tileID = 0;
            ownerMeeples[i].inUse = false;
            ownerMeeples[i].ownedRegion = NULL;

            if(i < MEEPLES_PER_PLAYER) //First player owner
            {
                this->availableMeeples[0]++; //Free up the meeple
            }
            else //Player 2
            {
                this->availableMeeples[1]++; //Free up the meeple
            }
        }
    }
    return 0;
}

int Regions::specialRemoveMeeple(unsigned int playerNumber, unsigned int tileID)
{
    for(unsigned int i = (playerNumber -  1)*(MEEPLES_PER_PLAYER); i < ((playerNumber - 1)*(MEEPLES_PER_PLAYER) + (MEEPLES_PER_PLAYER)); i++)
    {
        if((ownerMeeples[i].inUse) && (ownerMeeples[i].tileID == tileID))    // Meeple on tileID
        {
            ownerMeeples[i].inUse = false;
            ownerMeeples[i].ownedRegion = NULL;
            this->availableMeeples[playerNumber - 1]++;
            return 0; // success
        }
    }
    throw std::logic_error("No meeple of player exist on the requested tileID");
    return -1;
}

int Regions::checkOwner(unsigned int tileID, unsigned int edge, std::unordered_map<unsigned int, std::shared_ptr<struct regionSet> *> * tracker)
{
    if (tracker == NULL) tracker = &regionTracker;

    auto search = tracker->find(tileID);

    if (search == tracker->end()) throw std::logic_error("The tile can't be found in the regions.");

    if(search != tracker->end())
    {
        if((search->second[edge])->player1Meeples > (search->second[edge])->player2Meeples)
        {
            return OWNER_P1;
        }
        else if((search->second[edge])->player1Meeples < (search->second[edge])->player2Meeples)
        {
            return OWNER_P2;
        }
        else if((search->second[edge])->player1Meeples == (search->second[edge])->player2Meeples && ((search->second[edge])->player2Meeples) != 0)
        {
            return OWNER_TIE;
        }
    }
    return OWNER_NONE;
}

std::shared_ptr<struct regionSet> Regions::createRegion(const Tile& tile, unsigned int edge, TerrainType type) {
    std::shared_ptr<struct regionSet> newRegion = std::shared_ptr<struct regionSet>(new struct regionSet());
    std::shared_ptr<struct tileNode> node = std::shared_ptr<struct tileNode>(new struct tileNode());

    unsigned int numOfSides = tile.getNumberOfSides();
    unsigned int countPerSide = tile.getCountPerSide();
    unsigned int totalEdges = numOfSides * countPerSide;
    unsigned int edgesTillCompletion = 0;
    unsigned int centerEdge = countPerSide / 2;

    for (unsigned int otherEdge = 0; otherEdge < totalEdges; otherEdge++) {
        if (otherEdge % countPerSide == centerEdge && tile.isConnected(edge, otherEdge)) edgesTillCompletion++;
    }

    node->tileID = tile.getId();
    node->edge = edge;
    newRegion->type = type;
    newRegion->head = node;
    newRegion->tail = node;
    newRegion->edgesTillCompletion = edgesTillCompletion;
    return newRegion;
}

std::shared_ptr<struct regionSet> * Regions::getRegions(unsigned int tileID)
{
    auto tileSearch = regionTracker.find(tileID);
    if(tileSearch != regionTracker.end())
    {
        return (tileSearch->second);
    }

    return NULL;
}

bool Regions::ownerMeepleInUse(unsigned int meepleIndex)
{
    return ownerMeeples[meepleIndex].inUse;
}

unsigned int Regions::getMeepleTileId(unsigned int meepleIndex)
{
    return ownerMeeples[meepleIndex].tileID;
}

// This expects all 12 boardering tiles
struct moveResult Regions::tryMove(const Tile& tile, const Tile ** boarderingTiles, int meepleEdge, bool specialMeeple) {
    // create a new unordered_map to hold our potential move so that we don't actually make the changes to our game
    std::unordered_map<unsigned int, std::shared_ptr<struct regionSet> *> myTracker;
    // iterate through the current regions and copy them to our new one
    for (auto iter = regionTracker.begin(); iter != regionTracker.end(); iter++)
        myTracker.emplace(iter->first, iter->second);

    // add a connection using our test unordered_map so that changes aren't made to the actual game
    std::shared_ptr<struct regionSet> * testRegions = addConnection(tile, boarderingTiles, &myTracker);
    int meepleOwner = checkOwner(tile.getId(), meepleEdge, &myTracker);
    if (!specialMeeple && meepleEdge >= 0 && meepleEdge < 13) addMeeple(meepleOwner == OWNER_P1 ? 1 : 2, tile.getId(), meepleEdge, &myTracker);
    else if (specialMeeple) addMeepleSpecial(meepleOwner == OWNER_P1 ? 1 : 2, tile.getId());

    // get some basic tile info
    unsigned int numSides = tile.getNumberOfSides();
    unsigned int allSides = numSides * 2;
    unsigned int countPerSide = tile.getCountPerSide();
    unsigned int centerEdge = countPerSide / 2;
    unsigned int totalIndices = numSides * countPerSide;

    // iterate through the boardering tiles and check if we are surrounded
    unsigned int surrounding = 0;
    for (unsigned int i = 0; i < allSides; i++)
        if (boarderingTiles[i] != NULL) surrounding++;

    // create our new move result object
    struct moveResult result;
    // iterate through all of our tile's edges
    for (unsigned int edge = 0; edge < totalIndices; edge++) {
        // get the current side we are on and the side/edge opposite of us
        unsigned int side = edge / countPerSide;
        unsigned int correspondingSide = (side + (numSides / 2)) % numSides;
        unsigned int correspondingEdge = (countPerSide - (edge % countPerSide) - 1) + (countPerSide * correspondingSide);

        // if we are in the middle of a side, then we get the edgesTillCompletion
        if (edge % countPerSide == centerEdge) result.edgesTillCompletion += testRegions[edge]->edgesTillCompletion;

        // set our original score to zero
        unsigned int origScore = 0;

        // if there is a boardering tile for the current side, then we get the original score by scoring that tile's region using the actual game's regionTracker
        if (boarderingTiles[side] != NULL) origScore = game->rules->getCurrentScore(boarderingTiles[side]->getId(), correspondingEdge);

        // score the same region but now using our new test move's region and get the difference
        unsigned int score = game->rules->getCurrentScore(testRegions, edge, &tile, surrounding);
        int scoreDiff = score - origScore;

        // check who the owner of the region is and attribute them the points
        int owner = checkOwner(tile.getId(), edge, &myTracker);
        if (owner == OWNER_P1) result.player1ScoreChange += scoreDiff;
        else if (owner == OWNER_P2) result.player2ScoreChange += scoreDiff;
        else if (owner == OWNER_TIE) {
            result.player1ScoreChange += scoreDiff;
            result.player2ScoreChange += scoreDiff;
        }
    }

    if (!specialMeeple && meepleEdge >= 0 && meepleEdge < 13) removeMeeple(tile.getId(), meepleEdge, &myTracker);
    else if (specialMeeple) specialRemoveMeeple(meepleOwner == OWNER_P1 ? 1 : 2, tile.getId());

    // iterate through all of the unordered_map's elements deleting the regionSet arrays
    for (auto iter = myTracker.begin(); iter != myTracker.end(); iter++)
        delete[] iter->second;

    return result;
}

unsigned int Regions::meeplesAvailable(unsigned int playerNumber)
{
    if(playerNumber < 3)
    {
        return this->availableMeeples[playerNumber - 1];
    }
    return 0;
}

unsigned int Regions::crocsAvailable(unsigned int playerNumber)
{
    if(playerNumber < 3)
    {
        return this->availableCrocs[playerNumber - 1];
    }
    return 0;
}


#ifdef __testing

void Regions::clearRegionTracker() {
    regionTracker = std::unordered_map<unsigned int, std::shared_ptr<struct regionSet> *>();
}

bool Regions::checkRegionExistence(unsigned int tileID, unsigned int edge) {
    auto search = regionTracker.find(tileID);
    if(search != regionTracker.end())
    {
        if((search->second)[edge] != NULL)
        {
            return true;
        }
    }
    return false;
}

unsigned int Regions::checkRegionEdgesTillCompletion(unsigned int tileID, unsigned int edge) {
    auto search = regionTracker.find(tileID);
    if(search != regionTracker.end())
    {
        if((search->second)[edge] != NULL)
        {
            return ((search->second)[edge]->edgesTillCompletion);
        }
    }

    throw std::logic_error("The tile, edge combination could not be found.");
}

void Regions::clearOwnerMeeples() {
    for (int i = 0; i < TOTAL_MEEPLES; i++) {
        ownerMeeples[i].inUse = false;
        if (ownerMeeples[i].ownedRegion != NULL) {
            ownerMeeples[i].ownedRegion = std::shared_ptr<struct regionSet>(NULL);
        }
    }
}

#endif
