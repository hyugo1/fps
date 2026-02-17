//Game.h

#ifndef GAME_H
#define GAME_H

#include <vector>
#include "Enemy.h"
#include <SDL2/SDL.h>
#include "Entity.h"

class Menu;

class Game {
    std::vector<Enemy> enemies;
    Menu* menu;

    public:
        enum GameState {
            MENU,
            PLAYING,
            LEVEL_COMPLETE,
            GAME_OVER // wont kill the game
        };
        GameState currentState;
        int currentLevel;

        int playerHP;
        float playerInvulnTimer;

        Game();

        bool Init();
        bool IsRunning();
        void HandleEvents();
        void Update(float deltaTime);
        void UpdateMenu();
        void UpdateGame(float deltaTime);
        void UpdateLevelComplete();
        void UpdateGameOver();
        void Render();
        void Clean();
        void SpawnEnemies(int count);
        void RenderMenu();
        void RenderGame();
        void RenderLevelComplete();
        void RenderGameOver();
        // srand(time(nullptr));
        struct Bullet {
            float x, y;
            float dx, dy;
            float speed;
        };

        std::vector<Bullet> bullets;
        float shootCooldown;
        
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        bool running;
        Uint32 lastTime;
        
        Entity player;

        int screenWidth;
        int screenHeight;

        int cameraX;
        int cameraY;

        int tileSize;

        static const int mapWidth = 16;
        static const int mapHeight = 16;
        int map[mapWidth * mapHeight];

        void DetectMouseClick();
        void UpdateBullets(float deltaTime);
        void DrawMap();
        void DrawTile(int x, int y);
        bool DetectCollision(const Entity& entity, float nextX, float nextY);
        double Clamp(double a, double minimum, double maximum);
};

#endif // GAME_H