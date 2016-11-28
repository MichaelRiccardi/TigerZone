CC=g++
CFLAGS=-Wall -std=c++14
DCFLAGS=-g -D__testing
DEPS=TurnCoordinator.h ../AI/AI.h ../AI/FuzzyLogic.h ../../ProgramCoordinator/ProgramCoordinator.h
CLEAN=*.o

default: TurnCoordinator.o

tests: TurnCoordinator-Test.o

TurnCoordinator.o: TurnCoordinator.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
	cp -f TurnCoordinator.o ../../Compiled/TurnCoordinator.o

TurnCoordinator-Test.o: TurnCoordinator.cpp $(DEPS)
	$(CC) $(CFLAGS) $(DCFLAGS) -c $< -o $@
	cp -f TurnCoordinator-Test.o ../../Compiled/TurnCoordinator.o

clean:
	rm -rf $(CLEAN)
