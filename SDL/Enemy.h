//Enemy.h

#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>
#include "Entity.h"
#include <functional>

class Enemy {
    public:
        enum EnemyType { horizontalEnemy, verticalEnemy, smartEnemy };
        EnemyType character;
        
        Enemy(float startX, float startY, EnemyType type);

        void Update(float deltaTime, std::function<bool(const Entity&, float, float)> collisionFunc, float playerX, float playerY);
        void Render(float cameraX, float cameraY, SDL_Renderer* renderer);
        float GetX() const;
        float GetY() const;
        const Entity& getBody() const;
        void TakeDamage(int amount);
        bool IsDead() const;
        int GetHP() const;

    private:
        Entity body;
        float directionX;
        float directionY;
        double hitpoint;
        int tileSize;
        void HorizontalMove(float deltaTime, std::function<bool(const Entity&, float, float)> collisionFunc);
        void VerticalMove(float deltaTime, std::function<bool(const Entity&, float, float)> collisionFunc);
        void SmartEnemy(float deltaTime, std::function<bool(const Entity&, float, float)> collisionFunc, float playerX, float playerY);
};

#endif // enemy