// SDL2.cpp 

#include "Game.h"
#include "Enemy.h"
#include "Entity.h"
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
    Game game;
    if (game.Init()) {
        while (game.IsRunning()) {
            game.HandleEvents();
            game.Update();
            game.Render();
        }
    }
    game.Clean();
    return 0;
}