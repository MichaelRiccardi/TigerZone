#ifndef __TILE_H
#define __TILE_H

#include <string>
#include <algorithm>
#include "PreyType.h"
#include "TileType.h"
#include "TerrainType.h"
#include "../../Common/Array.h"

#define NUM_TILES                   28
#define NUM_TILE_SIDES              4
#define NUM_TILE_EDGES_PER_SIDE     3
#define NUM_TILE_EDGES              (NUM_TILE_EDGES_PER_SIDE * NUM_TILE_SIDES)

class Tile {
    public:
        /// Creates an array of tiles with the standard number of tiles for the game and assigns each tile a unique id
        static Array<Array<Tile>> CreateTiles();

        /// Creates each type of tile individually where the tile count is the number of tiles to create and the start id is the starting id which will be used to assign each tile's id. If start id is 0 and tile count is 2, then the first tile will have id 0 and the second tile will have id 1.
        static Array<Tile> CreateTileA(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileB(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileC(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileD(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileE(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileG(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileH(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileI(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileJ(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileK(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileL(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileN(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileP(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileR(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileT(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileU(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileV(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileW(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileX(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileY(unsigned int tileCount, unsigned int& startId, PreyType toPlace);
        static Array<Tile> CreateTileZ(unsigned int tileCount, unsigned int& startId, PreyType toPlace);

        /// The copy assignment operator which will create a new tile which is an exact copy of the provided tile.
        Tile& operator=(const Tile& other);
        friend bool operator==(const Tile& curr, const Tile& other);
        // The copy constructor which creates a new Tile which is an exact copy of the provided tile.
        Tile(const Tile& other);
        /// The tile destructor which frees the edges and edge connections arrays.
        ~Tile();

        friend std::ostream& operator<<(std::ostream& out, Tile tile);

        std::string getTileName() const;

        /// Sets the rotation of the current tile where each increment is a rotation of 90 degrees. So calling setRotation(2) will rotate the tile 180 degrees. After a tile is placed, a tile can no longer be rotated so the method returns true if the rotation was set and false otherwise.
        bool setRotation(unsigned int rotation);

        /// Since isConnected and getTerrainType account for the rotation already, you shouldn't need to use this function unless you are trying to display a tile on the UI.
        unsigned int getRotation() const;

        /// Returns a boolean indicating whether the tile contains a shield.
        PreyType getPrey() const;

        /// Gets the number of sides a tile has.
        unsigned int getNumberOfSides() const;

        /// Gets the number of array indices that belong to each side.
        unsigned int getCountPerSide() const;

        TerrainType getCenter() const;

        /// Returns the terrain type indicated at the specified edge index. For example, getTerrainType(2) will get the terrain type for the middle trisection of the top edge.
        TerrainType getTerrainType(unsigned int edge) const;

        /// Gets the tile type for the current tile.
        TileType getTileType() const;

        /// Takes two edge indices and outputs whether they are connected. Direction does not matter so isConnected(in, out) is equal to isConnected(out, int).
        bool isConnected(unsigned int inEdge, unsigned int outEdge) const;

        /// Sets the tile state to placed so that no modifications can be made to it.
        void placeTile();

        /// Returns true if the tile has been placed and false otherwise.
        bool isPlaced() const;

        /// Gets the id of the current tile.
        unsigned int getId() const;

    private:
        Tile();

        std::string * tileName;

        unsigned int tileId;
        // The number of sides this tile has
        unsigned int numberSides;
        unsigned int countPerSide;
        // Specifies whether this tile has a shield
        PreyType prey;
        // The type of tile
        TileType tileType;
        // An array of TerrainTypes that specifies what is in each part of each edge.
        // Each edge should have 3 array elements.
        TerrainType * edges;
        // Seperate variable to hold what is in the center
        TerrainType center;
        unsigned int defaultRotation;
        // Rotation is an offset to the edges and edge_connections array
        unsigned int rotation;
        // Edge Connections is an array of integer arrays where the index correlates to an edge part and the array of integers at that location contains all of the edge parts that it connects to.
        // This should be the same size as the edges array
        unsigned int * edgeConnections;
        // Determines if the tile has been placed (used).
        bool placed;
};

#endif
