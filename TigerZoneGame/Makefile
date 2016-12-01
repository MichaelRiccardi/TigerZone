CC=g++
CFLAGS=-Wall -std=c++14
DCFLAGS=-g -D__testing
DEPS= ../Core/Tiles/Tile.h ../Core/Tiles/TerrainType.h ../Core/Tiles/TileType.h ../Core/Tiles/PreyType.h ../Core/Tiles/TileStack.h ../Core/BoardRules/Rules.h ../Common/Array.h ../Core/BoardManager/Coord.h ../Core/BoardManager/Move.h ../Core/Board/Board.h
CLEAN=*.o

default: TigerZoneGame.o

tests: TigerZoneGame-Test.o

TigerZoneGame.o: TigerZoneGame.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
	cp -f TigerZoneGame.o ../Compiled/TigerZoneGame.o

Board-Test.o: Board.cpp $(DEPS)
	$(CC) $(CFLAGS) $(DCFLAGS) -c $< -o $@
	cp -f TigerZoneGame-Test.o ../Compiled/TigerZoneGame.o

clean:
	rm -rf $(CLEAN)
