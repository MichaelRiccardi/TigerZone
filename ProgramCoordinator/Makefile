CC=g++
CFLAGS=-Wall -std=c++14
DCFLAGS=-g -D__testing
DEPS=../Compiled/*.o ProgramCoordinator.h
CLEAN=T T-Test

default: T

tests: T-Test

T: ExternalClient.cpp $(DEPS)
	$(CC) $(CFLAGS) $< ../Compiled/*.o -o $@

T-Test.o: ExternalClient.cpp $(DEPS)
	$(CC) $(CFLAGS) $(DCFLAGS) $< -o $@

clean:
	rm -rf $(CLEAN)
