CC=g++
CFLAGS=-Wall -std=c++14 -g -D__testing -Wextra -pthread -isystem ../../gtest/internal
DEPS=../../../Core/Tiles/*.h
GTEST_HEADERS=../../gtest/*.h ../../gtest/internal/*.h
GTEST_DIR=../..
CLEAN=*.o

Tile-Test.o: Tile-Test.cpp $(DEPS) $(GTEST_HEADERS)
	$(CC) -I$(GTEST_DIR) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
	cp -f Tile-Test.o ../../Compiled/Tile-Test.o

clean:
	rm -rf $(CLEAN)
