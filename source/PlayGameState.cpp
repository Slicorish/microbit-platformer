#include "PlayGameState.h"
#include "GameOverGameState.h"

namespace platformer {

    PlayGameState::PlayGameState(platformer::Game *game) : game(game) {}

    PlayGameState::~PlayGameState() {
        delete player;
        delete world;
    }

    void PlayGameState::onButtonAPress() {
        jump();
    }

    void PlayGameState::onButtonBPress() {
        jump();
    }

    void PlayGameState::jump() {
        BlockType below = world->getRelativeBlock(*player->getLocation(), 0, 1);
        if (below == FOREGROUND) {
            player->jump();
        }
    }

    void PlayGameState::run() {
        score = 0;
        player->getLocation()->set(1, 5);

        while (game->getState() == this) {
            game->getScreen()->clear();
            tick();
            render();
            game->getMicroBit()->display.image.paste(*game->getScreen());
            game->getMicroBit()->sleep(TICK_RATE);
        }

        delete this;
    }

    void PlayGameState::tick() {
        Vector2i *location = player->getLocation();
        Vector2i *velocity = player->getVelocity();
        Vector2i relativeCenter = world->getRelativeLocation(*player->getLocation(), 0, 0);

        BlockType center = world->getBlock(relativeCenter);
        BlockType below = world->getRelativeBlock(*player->getLocation(), 0, 1);
        BlockType left = world->getRelativeBlock(*player->getLocation(), -1, 0);
        BlockType right = world->getRelativeBlock(*player->getLocation(), 1, 0);

        if (center == COIN_OFF || center == COIN_ON) {
            world->setBlock(relativeCenter, AIR);
            score++;
        }

        if (below != FOREGROUND) {
            velocity->addY(-1);
        }

        int accelerometerX = game->getMicroBit()->accelerometer.getX();

        if (accelerometerX < -300 && location->getX() > 0 && left != FOREGROUND) {
            location->addX(-1);
        }

        if (accelerometerX > 300 && location->getX() < (world->getMaxX() - 2) && right != FOREGROUND) {
            location->addX(1);
        }

        if (velocity->getY() > 0) {
            location->addY(1);
        } else if (velocity->getY() < 0) {
            if (below == FOREGROUND) {
                velocity->setY(0);
            } else {
                location->addY(-1);

                if (location->getY() < 0) {
                    auto *nextState = new GameOverGameState(game);
                    game->setState(nextState);
                    return;
                }
            }
        }

        if (velocity->getX() > 0) {
            location->addX(1);
        } else if (velocity->getX() < 0) {
            location->addX(-1);
        }
    }

    void PlayGameState::render() {
        // Render the player.
        Vector2i *location = player->getLocation();
        int offsetX = HALF_SCREEN;
        int offsetY = HALF_SCREEN;

        if (location->getY() <= HALF_SCREEN) {
            offsetY += HALF_SCREEN - location->getY();
        }

        if (location->getY() >= (world->getMaxY() - HALF_SCREEN)) {
            offsetY -= HALF_SCREEN - ((world->getMaxY() - 1) - location->getY());
        }

        if (location->getX() <= HALF_SCREEN) {
            offsetX -= HALF_SCREEN - location->getX();
        }

        if (location->getX() >= (world->getMaxX() - HALF_SCREEN - 1)) {
            offsetX += HALF_SCREEN - ((world->getMaxX() - 2) - location->getX());
        }

        game->getScreen()->setPixelValue((uint16_t) offsetX, (uint16_t) offsetY, 255);

        // Render the map.
        for (int x = 0; x < SCREEN_SIZE; x++) {
            for (int y = 0; y < SCREEN_SIZE; y++) {
                renderBlock(offsetX, offsetY, x, y);
            }
        }
    }

    void PlayGameState::renderBlock(int offsetX, int offsetY, int x, int y) const {
        Vector2i blockLocation = world->getRelativeLocation(*player->getLocation(), x - offsetX, y - offsetY);
        BlockType blockType = world->getBlock(blockLocation);

        switch (blockType) {
            case AIR:
                break;
            case FOREGROUND:
            case BACKGROUND:
                game->getScreen()->setPixelValue((uint16_t) x, (uint16_t) y, 255);
                break;
            case COIN_ON:
                game->getScreen()->setPixelValue((uint16_t) x, (uint16_t) y, 255);
                world->setBlock(blockLocation, COIN_OFF);
                break;
            case COIN_OFF:
                world->setBlock(blockLocation, COIN_ON);
                break;
        }
    }

}