CC=g++
CFLAGS=-Wall -std=c++14 -pthread -lpthread
DCFLAGS=-g -D__testing
DEPS=ProgramCoordinator.h ../ExternalPlayers/TurnCoordinator/TurnCoordinator.h
CLEAN=*.o ExternalClient

default: ExternalClient

tests:

ExternalClient: ExternalClient.cpp $(DEPS)
	$(CC) $(CFLAGS) $< -o $@
	cp -f ExternalClient ../ExternalClient

#ProgramCoordinator-Test.o: ProgramCoordinator.cpp $(DEPS)
#	$(CC) $(CFLAGS) $(DCFLAGS) -c $< -o $@
#	cp -f ProgramCoordinator-Test.o ../Compiled/ProgramCoordinator.o

clean:
	rm -rf $(CLEAN)
