#ifndef __REGIONS_H
#define __REGIONS_H

#include "../../TigerZoneGame/TigerZoneGame.h"
#include "Rules.h"
#include "../BoardManager/Move.h"
#include "../Tiles/Tile.h"
#include <unordered_map>
#include <memory>

#define MEEPLES_PER_PLAYER   7
#define TOTAL_MEEPLES   (MEEPLES_PER_PLAYER * 2)

#define CROCS_PER_PLAYER   2
#define TOTAL_CROCS        (CROCS_PER_PLAYER * 2)

#define OWNER_P1        1
#define OWNER_TIE       0
#define OWNER_P2        -1
#define OWNER_NONE      -2

struct moveResult
{
    unsigned int edgesTillCompletion;
    int player1ScoreChange;
    int player2ScoreChange;
};

struct tileNode
{
    tileNode() {
        preyCounts = new unsigned int[NUM_PREY];
        tileID = -1;
        edge = -1;
        previous = std::shared_ptr<struct tileNode>(NULL);
        next = std::shared_ptr<struct tileNode>(NULL);
    };

    tileNode(const struct tileNode& other) : tileID(other.tileID), edge(other.edge) {
        std::copy(other.preyCounts, other.preyCounts + NUM_PREY, this->preyCounts);
        this->previous = std::shared_ptr<struct tileNode>(new struct tileNode(*other.previous));
        this->next = std::shared_ptr<struct tileNode>(new struct tileNode(*other.next));
    };

    struct tileNode& operator=(const struct tileNode& other) {
        this->tileID = other.tileID;
        this->edge = other.edge;
        std::copy(other.preyCounts, other.preyCounts + NUM_PREY, this->preyCounts);
        this->previous = std::shared_ptr<struct tileNode>(new struct tileNode(*other.previous));
        this->next = std::shared_ptr<struct tileNode>(new struct tileNode(*other.next));

        return *this;
    };

    ~tileNode() {
        delete[] preyCounts;
    }

    int tileID;
    int edge;
    unsigned int * preyCounts;
    std::shared_ptr<struct tileNode> previous;
    std::shared_ptr<struct tileNode> next;
};

struct regionSet
{
    regionSet() {
        player1Meeples = 0;
        player2Meeples = 0;
        edgesTillCompletion = 0;
        hasCroc = false;
    };

    regionSet(const struct regionSet& other) : player1Meeples(other.player1Meeples), player2Meeples(other.player2Meeples), type(other.type) {
        this->head = std::shared_ptr<struct tileNode>(new struct tileNode(*head));
        this->tail = std::shared_ptr<struct tileNode>(new struct tileNode(*tail));
    };

    struct regionSet& operator=(const struct regionSet& other) {
        this->player1Meeples = other.player1Meeples;
        this->player2Meeples = other.player2Meeples;
        this->head = std::shared_ptr<struct tileNode>(new struct tileNode(*head));
        this->tail = std::shared_ptr<struct tileNode>(new struct tileNode(*tail));

        return *this;
    };

    unsigned int player1Meeples;
    unsigned int player2Meeples;
    int edgesTillCompletion;
    TerrainType type;
    std::shared_ptr<struct tileNode> head;
    std::shared_ptr<struct tileNode> tail;
    bool hasCroc;
};

struct meeple
{
    meeple()
    {
        tileID = 0;
        ownedRegion = NULL;
        inUse = false;
    };
    unsigned int tileID;
    bool inUse;
    std::shared_ptr<struct regionSet> ownedRegion;
};

struct croc
{
    croc()
    {
        ownedRegions = NULL;
        inUse = false;
    };

    bool inUse;
    std::shared_ptr<struct regionSet> *ownedRegions;
};

class Regions
{
    public:
         Regions(TigerZoneGame* game);
         std::shared_ptr<struct regionSet> * addConnection(const Tile& newTile, const Tile **  boarderingTiles, std::unordered_map<unsigned int, std::shared_ptr<struct regionSet> *> * tracker = NULL);
         int addMeeple(unsigned int playerNumber, unsigned int tileID, unsigned int edge, std::unordered_map<unsigned int, std::shared_ptr<struct regionSet> *> * tracker = NULL);
         int addMeepleSpecial(unsigned int playerNumber, unsigned int tileID);
         int specialRemoveMeeple(unsigned int playerNumber, unsigned int tileID);
         int checkOwner(unsigned int tileID, unsigned int edge, std::unordered_map<unsigned int, std::shared_ptr<struct regionSet> *> * tracker = NULL);
         std::shared_ptr<struct regionSet> * getRegions(unsigned int tileID);
         int removeMeeple(unsigned int tileID, unsigned int edge, std::unordered_map<unsigned int, std::shared_ptr<struct regionSet> *> * tracker = NULL);
         bool checkRegionExistence(unsigned int tileID, unsigned int edge);
         unsigned int checkRegionEdgesTillCompletion(unsigned int tileID, unsigned int edge);
         struct moveResult tryMove(const Tile& tile, const Tile ** boarderingTiles, int meepleEdge = -1, bool specialMeeple = false);

         unsigned int meeplesAvailable(unsigned int playerNumber);
         bool ownerMeepleInUse(unsigned int meepleIndex);
         unsigned int getMeepleTileId(unsigned int meepleIndex);

         int addCroc(unsigned int playerNumber, unsigned int tileID);
         unsigned int crocsAvailable(unsigned int playerNumber);

#ifdef __testing
         void clearRegionTracker();
         void clearOwnerMeeples();
#else
    private:
#endif
         TigerZoneGame* game;

         std::shared_ptr<struct regionSet> createRegion(const Tile& tile, unsigned int edge, TerrainType type);
         int countEdgesTillCompletion(unsigned int placedTileID);
         void mergeRegions(unsigned int placedTileID, unsigned int placedEdge, unsigned int connectingTileID, unsigned int connectingEdge);

         std::unordered_map<unsigned int, std::shared_ptr<struct regionSet> *> regionTracker;    //Takes (tileID, edge) returns set pointer

         struct meeple ownerMeeples[TOTAL_MEEPLES];
         unsigned int availableMeeples[2];

         struct croc ownerCrocs[TOTAL_CROCS];
         unsigned int availableCrocs[2];
};

#endif
