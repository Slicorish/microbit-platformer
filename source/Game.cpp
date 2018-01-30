#include "Game.h"

namespace platformer {

    namespace {
        const int mapX = 10;
        const int mapY = 7;

        char map[mapY][mapX] = {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 1, 1, 1, 0, 0, 0, 1},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
        };

        // This is an incredibly gross hack to get things working. Apparently
        // the API dislikes function pointers to class members.
        // TODO: Figure out how to implement this more elegantly.
        Game *game;

        void jumpHelper(MicroBitEvent event) {
            game->jump(event);
        }

        void gameLoopHelper() {
            game->gameLoop();
        }
    }

    void Game::gameOver() {
        microBit->display.clear();
        microBit->display.scroll("GAME OVER! SCORE:");
        microBit->display.scroll(score);
    }

    void Game::jump(MicroBitEvent) {
        player->jump();
    }

    void Game::tick() {
        Vector2i *location = player->getLocation();
        Vector2i *velocity = player->getVelocity();

        if (!player->isOnGround()) {
            velocity->addY(-1);
        }

        int accelerometerX = microBit->accelerometer.getX();

        if (accelerometerX < -300 && location->getX() > 0) {
            location->addX(-1);
        }

        if (accelerometerX > 300 && location->getX() < (mapX - HALF_SCREEN)) {
            location->addX(1);
        }

        if (velocity->getY() > 0) {
            location->addY(1);
        } else if (velocity->getY() < 0) {
            location->addY(-1);

            if (location->getY() < 0) {
                location->setY(0);
                velocity->setY(0);
            }
        }

        if (velocity->getX() > 0) {
            location->addX(1);
        } else if (velocity->getX() < 0) {
            location->addX(-1);
        }

        microBit->sleep(TICK_RATE);
    }

    void Game::render() {
        // Ensure the screen is clear.
        screen->clear();

        // Render the player.
        Vector2i *location = player->getLocation();
        screen->setPixelValue(HALF_SCREEN, HALF_SCREEN, 255);

        if (location->getY() <= HALF_SCREEN) {
            auto shift = (uint16_t) (HALF_SCREEN - location->getY() - 1);
            screen->shiftDown(shift);
        }

        if (location->getY() > mapY) {
            auto shift = (uint16_t) (location->getY() - mapY);
            screen->shiftUp(shift);
        }

        if (location->getX() < HALF_SCREEN) {
            auto shift = (uint16_t) (HALF_SCREEN - location->getX());
            screen->shiftLeft(shift);
        }

        if (location->getX() > (mapX - SCREEN_SIZE)) {
            auto shift = (uint16_t) (location->getX() - (mapX - SCREEN_SIZE));
            screen->shiftRight(shift);
        }

        // Render the map.
        for (int x = -SCREEN_SIZE; x <= SCREEN_SIZE; x++) {
            for (int y = -SCREEN_SIZE; y <= SCREEN_SIZE; y++) {
                // Get the relative map coordinates for the players position.
                int relative_x = (x) + location->getX();
                int relative_y = (HALF_SCREEN + y) - location->getY();

                // Do not render unmapped coordinates.
                if (relative_x < 0 || relative_x >= mapX || relative_y < 0 || relative_y >= mapY) {
                    continue;
                }

                if (map[relative_y][relative_x] == 1) {
                    screen->setPixelValue((uint16_t) x, (uint16_t) y, 255);
                }
            }
        }

//        if (location->getY() <= HALF_SCREEN) {
//            uint16_t shift = (uint16_t) (HALF_SCREEN - location->getY() - 1);
//            screen->shiftDown(shift);
//        }
//
//        if (location->getY() > mapY) {
//            uint16_t shift = (uint16_t) location->getY() - (uint16_t) mapY;
//            screen->shiftUp(shift);
//        }
//
//        if (location->getX() < HALF_SCREEN) {
//            uint16_t shift = (uint16_t) (HALF_SCREEN - location->getX());
//            screen->shiftLeft(shift);
//        }
//
//        if (location->getY() > mapX) {
//            uint16_t shift = (uint16_t) location->getX() - (uint16_t) mapX;
//            screen->shiftRight(shift);
//        }
    }

    void Game::gameLoop() {
        while (!state) {
            tick();
            render();
        }
    }

    void Game::run() {
        // Reset all game state.
        state = 0;
        score = 0;
        player->getLocation()->set(2, 3);
        screen->clear();

        // Spawn fiber to handle the game loop.
        game = this;
        create_fiber(gameLoopHelper);

        // Register event handlers for button presses.
        microBit->messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, jumpHelper);
        microBit->messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, jumpHelper);

        // Now just keep the screen refreshed.
        while (!state) {
            microBit->display.image.paste(*screen);
            microBit->sleep(10);
        }

        // Display GAME OVER and score
        gameOver();
    }

    Game::~Game() {
        delete microBit;
        delete screen;
        delete player;
    }

    MicroBit *Game::getMicroBit() const {
        return microBit;
    }

    MicroBitImage *Game::getScreen() const {
        return screen;
    }

    Player *Game::getPlayer() const {
        return player;
    }

    int Game::getState() const {
        return state;
    }

    void Game::setState(int state) {
        Game::state = state;
    }

    int Game::getScore() const {
        return score;
    }

    void Game::setScore(int score) {
        Game::score = score;
    }

    bool Game::complete() {
        return false;
    }

} // namespace platformer
