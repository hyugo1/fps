//Game.h

#ifndef GAME_H
#define GAME_H

#include <vector>
#include "Enemy.h"
#include <SDL2/SDL.h>
#include "Entity.h"
#include "Weapon.h"
#include "Items.h"

class Menu;

class Game {
    Menu* menu;
    public:
        Game();
        bool Init();
        bool IsRunning();
        void HandleEvents();
        void Update();
        void Render();
        void Clean();
        
        
    private:
        // ====== Game State ======
        enum GameState { MENU, OPTIONS, PLAYING, PAUSED, LEVEL_COMPLETE, GAME_OVER };
        enum Difficulty { DEFAULT, EASY, MEDIUM, HARD };
        Difficulty currentDifficulty = MEDIUM;
        float GetDifficultyMultiplier() const;
        GameState currentState;
        GameState previousState;
        int currentLevel;
        int getLevel();
        Uint32 getLastTime();
        GameState getCurrentState();
        float getDeltaTime();

        // ====== Player ======
        Entity player;
        int playerHP;
        int playerMaxHP;
        float playerInvulnTimer;
        float playerBaseSpeed;
        float playerSpeed;
        int playerMeleeDamage;

        // ====== Enemies ======
        std::vector<Enemy> enemies;
        void UpdateEnemy(float deltaTime);
        void EnemyHP();

        // ====== Bullets ======
        std::vector<Bullet> bullets;
        float shootCooldown;
        
        void UpdateBullets(float deltaTime);
        void DetectMouseClick();
        
        // ==== Weapons ====
        std::vector<Weapon> playerWeapons;
        int currentWeaponIndex = 0;
        bool inventoryOpen = false;
        void UpdateReloadCooldown(float deltaTime);
        void HandleReloadInput();

        // ==== Items ====
        std::vector<HealthItem> healthItems;
        std::vector<WeaponItem> weaponItems;
        std::vector<SpeedItem> speedItems;
        void UpdateHealthItems();
        void UpdateWeaponItems();
        void UpdateSpeedItems(float deltaTime);

        float speedItemDuration;
        float speedItemTimer;
        float speedItemAmount;
        bool speedItemActive;

        
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

        // ====== Animation ======
        float shootAnimTimer; // time left for shooting animation, used to show muzzle flash and prevent weapon switching during animation
        float shootAnimDuration;
        float lastShotDirX; // direction of last shot, used for recoil animation
        float lastShotDirY;
        bool playerDying;
        float playerDeathTimer; // time left for player death animation, used to show death animation and prevent input during it
        float playerDeathDuration;
        
        // ====== Game Systems ======
        void UpdateGame(float deltaTime, float dx, float dy);
        
        // ====== Player Systems ======
        void HandlePlayerInput(float deltaTime, float &dx, float &dy);
        void HandlePlayerMovementInput(float deltaTime, float &dx, float &dy);
        void HandleInventoryInput();
        void UpdateWeaponCooldown(float deltaTime);
        void UpdatePlayer(float deltaTime);
        void UpdateCollision(float deltaTime, float dx, float dy);
        void PlayerHP();
        void DisplayAmmo();
        void DisplayScore();
        void LoadHighScore();
        void SaveHighScore();
        void ResetHighScore();

        // ====== Menu / Rendering ======
        void UpdateMenu();
        void UpdateOptionsMenu();
        void UpdatePlayingGameState(float deltaTime);
        void UpdateLevelComplete();
        void UpdateGameOver();
        void HandlePauseInput();
        void RenderPauseOverlay();
        void RenderMenu();
        void RenderOptionsMenu();
        void RenderPauseMenu();
        void RenderGameScene();
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
        int score;
        int highScore;
        bool highScoreResetInGameOver;

        double Clamp(double a, double min, double max);
    };

#endif // GAME_H