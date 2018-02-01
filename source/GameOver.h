#ifndef PLATFORMER_GAME_OVER_GAME_STATE_H
#define PLATFORMER_GAME_OVER_GAME_STATE_H

#include "Game.h"

namespace platformer {

    class GameOver : public GameState {
    private:
        static const int TICK_RATE = 200;
        static const int GAME_OVER_FLASHES = 5;

        Game *game;
        int gameOverTicks = 0;
    public:
        explicit GameOver(Game *game);

        void onButtonAPress() override;

        void onButtonBPress() override;

        void run() override;

        void tick();

        void render() const;
    };

}

#endif //PLATFORMER_GAME_OVER_GAME_STATE_H