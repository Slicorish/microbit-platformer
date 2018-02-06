#include "Game.h"

int main() {
//    // Create and run the game.
//    platformer::Game game;
//    game.run();
//    return EXIT_SUCCESS;
    // Create and run the game.
    int i1 = 0, i2 = 1, i3 = 2, i4 = 3, i5 = 4, i6 = 5, i7 = 6, i8 = 7, i9 = 8;
    auto game = new platformer::Game();
    game->run();

    game->getMicroBit()->display.print(i1);
    game->getMicroBit()->display.print(i2);
    game->getMicroBit()->display.print(i3);
    game->getMicroBit()->display.print(i4);
    game->getMicroBit()->display.print(i5);
    game->getMicroBit()->display.print(i6);
    game->getMicroBit()->display.print(i7);
    game->getMicroBit()->display.print(i8);
    game->getMicroBit()->display.print(i9);

    // Cleanup and exit.
    delete game;
    return EXIT_SUCCESS;
}
