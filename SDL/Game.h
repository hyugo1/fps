//Game.h

#ifndef GAME_H
#define GAME_H

#include <vector>
#include "Enemy.h"
#include <SDL2/SDL.h>

class Game {
    std::vector<Enemy> enemies;

    public:
        Game();

        bool Init();
        bool IsRunning();
        void HandleEvents();
        void Update(float deltaTime);
        void Render();
        void Clean();

    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        bool running;
        Uint32 lastTime;
        float playerX;
        float playerY;

        int screenWidth;
        int screenHeight;

        int cameraX;
        int cameraY;

        int tileSize;

        static const int mapWidth = 16;
        static const int mapHeight = 16;
        int map[mapWidth * mapHeight];

        void DrawMap();
        void drawTile(int x, int y);
        bool detectCollision(float x, float y);
        double clamp(double a, double minimum, double maximum);
};

#endif // GAME_H