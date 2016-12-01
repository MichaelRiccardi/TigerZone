CC=g++
CFLAGS=-Wall -std=c++14
DCFLAGS=-g -D__testing
DEPS=../Tiles/Tile.h ../Tiles/TerrainType.h ../Tiles/TileType.h ../Tiles/PreyType.h ../Tiles/TileStack.h ../../Common/Array.h ../BoardManager/Coord.h ../BoardManager/Move.h ../../TigerZoneGame/TigerZoneGame.h Board.h
CLEAN=*.o

default: Board.o

tests: Board-Test.o

Board.o: Board.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
	cp -f Board.o ../../Compiled/Board.o

Board-Test.o: Board.cpp $(DEPS)
	$(CC) $(CFLAGS) $(DCFLAGS) -c $< -o $@
	cp -f Board-Test.o ../../Compiled/Board.o

clean:
	rm -rf $(CLEAN)
