//Enemy.h

#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>
#include "Entity.h"
#include <functional>

class Enemy {
    public:
        enum EnemyType { horizontalEnemy, verticalEnemy, smartEnemy };
        using CollisionFunc = std::function<bool(const Entity&, float, float)>;
        struct UpdateContext {
            float deltaTime;
            float playerX;
            float playerY;
            const CollisionFunc& collisionFunc;
        };
        EnemyType character;
        
        Enemy(float startX, float startY, EnemyType type, int level, float difficultyMultiplier);
        static void ReleaseTextures();

        void Update(const UpdateContext& context);
        void Render(float cameraX, float cameraY, SDL_Renderer* renderer);
        float GetX() const;
        float GetY() const;
        const Entity& getBody() const;
        void TakeDamage(int amount);
        bool IsDead() const;
        bool IsDying() const;
        bool IsRemovable() const;
        int GetHP() const;
        int GetMaxHP() const;
        float GetSpeed() const;
    private:
        static SDL_Texture* horizontalTexture;
        static SDL_Texture* verticalTexture;
        static SDL_Texture* smartTexture;
        static bool texturesLoaded;
        static void EnsureTexturesLoaded(SDL_Renderer* renderer);
        SDL_Rect DrawEnemyRectangle(float cameraX, float cameraY) const;
        SDL_Texture* currentEnemyTexture = nullptr;
        Uint8 baseR = 255;
        Uint8 baseG = 255;
        Uint8 baseB = 255;
        Entity body;
        float directionX;
        float directionY;
        int tileSize;
        int health;
        int maxHealth;
        float speed;
        float maxdistance;
        bool isDying;
        float deathTimer;
        float deathDuration;
        void HorizontalMove(const UpdateContext& context);
        void VerticalMove(const UpdateContext& context);
        void SmartEnemy(const UpdateContext& context);
        bool CheckIfDying(const UpdateContext& context);
        void RenderAliveEnemy(float cameraX, float cameraY, SDL_Renderer* renderer);
        void RenderDeathEffect(float cameraX, float cameraY, SDL_Renderer* renderer) const;
        void SetEnemyTextureAndColor();
        float GetProgress() const;
        float GetDistanceToPlayer(float dx, float dy) const;
        void UpdateMovementByType(const UpdateContext& context);
    };
    
#endif // enemy