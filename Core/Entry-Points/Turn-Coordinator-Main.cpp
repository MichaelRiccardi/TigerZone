#include "../../TigerZoneGame/TigerZoneGame.h"
#include "../../ExternalPlayers/TurnCoordinator/TurnCoordinator.h"
#include <stdexcept>

class TigerZoneGame;

int main(int argc, char * argv[]) {
    if (argc < 1) throw std::invalid_argument("Not enough parameters specified.");
    int port = atoi(argv[0]);
    /*TigerZoneGame* game = new TigerZoneGame();
    TurnCoordinator coordinator(port, game);
    coordinator.startCoordinator();*/
}
