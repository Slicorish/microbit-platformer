#include "MenuGameState.h"
#include "PlayGameState.h"
#include "ConnectGameState.h"

namespace platformer {

    MenuGameState::MenuGameState(Game *game) : game(game) {}

    void MenuGameState::onButtonAPress() {
        game->getMicroBit()->display.stopAnimation();
    }

    void MenuGameState::onButtonBPress() {
        // Stop all current screen animations.
        game->getMicroBit()->display.stopAnimation();

        // Update the next state to a game session in the currently selected world.
        auto *nextState = new PlayGameState(game, createWorld(selectedWorld));
        game->setState(nextState);

        // If in multiplayer, tell our partner to join the selected world.
        if (game->isMultiplayer()) {
            ByteBuf out = game->createPacket();
            out.writePacketType(PacketType::SELECT_WORLD);
            out.writeInt(selectedWorld);
            game->sendPacket(out);
        }
    }

    void MenuGameState::onButtonABPress() {
        // Toggle whether the game is in multiplayer mode.
        game->setMultiplayer(!game->isMultiplayer());

        // Stop all active screen animation.
        game->getMicroBit()->display.stopAnimation();

        if (game->isMultiplayer()) {

            // Switch to the connect game state.
            auto *nextState = new ConnectGameState(game);
            game->setState(nextState);

        } else if (game->isConnected()) {

            // Disconnect from partner.
            game->disconnect();

        }
    }

    void MenuGameState::onMessage(ByteBuf &in) {
        PacketType packetType = in.readPacketType();

        switch (packetType) {
            case PacketType::SELECT_WORLD: {
                selectedWorld = in.readInt();
                game->getMicroBit()->display.stopAnimation();

                auto *nextState = new PlayGameState(game, createWorld(selectedWorld));
                game->setState(nextState);
                return;
            }
            case PacketType::DISCONNECT: {
                game->disconnect();
                return;
            }
            default: {
                return;
            }
        }
    }

    void MenuGameState::run() {
//        game->getMicroBit()->display.scroll("SELECT WORLD", SCROLL_SPEED);

        while (game->getState() == this) {
            game->getScreen()->clear();
            tick();
//            render();
            game->getMicroBit()->sleep(TICK_RATE);
        }

        delete this;
    }

    void MenuGameState::tick() {
        int accelerometerX = game->getMicroBit()->accelerometer.getX();

        if (accelerometerX < -300) {
            selectedWorld--;

            if (selectedWorld <= 0) {
                selectedWorld = WORLD_COUNT;
            }
        }

        if (accelerometerX > 300) {
            selectedWorld++;

            if (selectedWorld > WORLD_COUNT) {
                selectedWorld = 1;
            }
        }
    }

    void MenuGameState::render() const {
        game->getMicroBit()->display.print(selectedWorld);
    }

}
