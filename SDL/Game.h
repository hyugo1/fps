//Game.h

#ifndef GAME_H
#define GAME_H

#include <vector>
#include "Enemy.h"
#include <SDL2/SDL.h>
#include "Entity.h"

class Menu;

class Game {
    Menu* menu;
    public:
        Game();
        bool Init();
        bool IsRunning();
        void HandleEvents();
        void Update(float deltaTime);
        void Render();
        void Clean();

    private:
        // ====== Game State ======
        enum GameState { MENU, PLAYING, LEVEL_COMPLETE, GAME_OVER };
        GameState currentState;
        int currentLevel;
        int getLevel();

        // ====== Player ======
        Entity player;
        int playerHP;
        int playerMaxHP;
        float playerInvulnTimer;

        // ====== Enemies ======
        std::vector<Enemy> enemies;
        void SpawnEnemies(int count);
        void UpdateEnemy(float deltaTime);
        void EnemyHP();

        // ====== Bullets ======
        struct Bullet {
            float x, y;
            float dx, dy;
            float speed;
            bool toDelete = false;
        };
        std::vector<Bullet> bullets;
        float shootCooldown;
        
        void UpdateBullets(float deltaTime);
        void DetectMouseClick();

        // ==== Items ====
        struct HealthItem {
            float x, y;
            float width, height;
            bool collected = false;
        };
        std::vector<HealthItem> healthItems;
        void SpawnHealthItems(int count);
        void UpdateHealthItems();

        
        // ====== Map ======
        static const int mapWidth = 16;
        static const int mapHeight = 16;
        int map[mapWidth * mapHeight];
        int tileSize;
        
        void DrawMap();
        void DrawTile(int x, int y);
        bool DetectCollision(const Entity& entity, float nextX, float nextY);
        
        // ====== Camera ======
        int cameraX;
        int cameraY;
        void UpdateCamera(float deltaTime, float dx, float dy);
        void UpdateClamp();
        
        // ====== Game Systems ======
        void UpdateGame(float deltaTime, float dx, float dy);
        
        // ====== Player Systems ======
        void HandlePlayerInput(float deltaTime, float &dx, float &dy);
        void UpdatePlayer(float deltaTime);
        void UpdateCollision(float deltaTime, float dx, float dy);
        void PlayerHP();

        // ====== Menu / Rendering ======
        void UpdateMenu();
        void UpdateLevelComplete();
        void UpdateGameOver();
        void RenderMenu();
        void RenderGame();
        void RenderLevelComplete();
        void RenderGameOver();

        // ====== SDL ======
        SDL_Window* window;
        SDL_Renderer* renderer;
        bool running;
        Uint32 lastTime;
        int screenWidth;
        int screenHeight;

        double Clamp(double a, double min, double max);
    };

#endif // GAME_H