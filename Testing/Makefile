CC=g++
CPPFLAGS=-isystem gtest/include
CFLAGS=-g -Wall -Wextra -pthread -lpthread
DEPS=Compiled/Tile-Test.o ../Compiled/Tile.o ../Compiled/TileStack.o ../Compiled/Board.o Compiled/Board-Test.o ../Compiled/BoardManager.o Compiled/BoardManager-Test.o \
	../Compiled/Move.o ../Compiled/Coord.o ../Compiled/Rules.o ../Compiled/Regions.o Compiled/Fuzzy-Test.o ../Compiled/FuzzyLogic.o ../Compiled/TurnCoordinator.o \
	Compiled/TurnCoordinator-Test.o ../Compiled/AI.o Compiled/Board-Rules-Test.o Compiled/TigerZoneGame.o
UNAME_S := $(shell uname -s)
GCCVERSION = $(shell gcc --version | grep ^gcc | sed 's/^.* //g')

ifeq ($(UNAME_S), Linux)
ifeq "$(GCCVERSION)" "4.9.4"
	GTEST_DEPS=gtest/gtest_main_g++4.9.a
else
	GTEST_DEPS=gtest/gtest_main.a
endif
endif
ifeq ($(UNAME_S), Darwin)
	GTEST_DEPS=gtest/gtest_main_mac.a
endif


default:
	mkdir -p Compiled
	$(MAKE) -C ./Core
	$(MAKE) -C ./ExternalPlayers
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPS) $(GTEST_DEPS)  -o tests
	./tests --gtest_output=xml:test_result.xml || /bin/true

clean:
	$(MAKE) -C ./Core -f Makefile clean
	$(MAKE) -C ./ExternalPlayers -f Makefile clean
	rm -rf Compiled/*
