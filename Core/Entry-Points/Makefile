CC=g++
CFLAGS=-Wall -std=c++14
DCFLAGS=-g -D__testing
DEPS=../../ExternalPlayers/TurnCoordinator/TurnCoordinator.h
CLEAN=*.o

default: TurnCoordinator-Main.o

tests: #TurnCoordinator-Main-Test.o

TurnCoordinator-Main.o: Turn-Coordinator-Main.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
	cp -f TurnCoordinator-Main.o ../../Compiled/TurnCoordinator-Main.o

TurnCoordinator-Main-Test.o: Turn-Coordinator-Test.cpp $(DEPS)
	$(CC) $(CFLAGS) $(DCFLAGS) -c $< -o $@
	cp -f TurnCoordinator-Test.o ../../Compiled/TurnCoordinator-Main.o

clean:
	rm -rf $(CLEAN)
