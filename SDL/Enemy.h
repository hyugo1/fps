//Enemy.h

#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>
#include "Entity.h"

class Enemy {
    public:
        enum EnemyType { horizontalEnemy, verticalEnemy, smartEnemy };
        EnemyType character;
        
        Enemy(float startX, float startY, EnemyType type);

        void Update(float deltaTime, int map[], int mapWidth, int mapHeight, float playerX, float playerY);
        void Render(float cameraX, float cameraY, SDL_Renderer* renderer);
        float getX() const;
        float getY() const;
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
        bool detectCollision(float x, float y, int map[], int mapWidth, int mapHeight);
        void HorizontalMove(float deltaTime, int map[], int mapWidth, int mapHeight);
        void VerticalMove(float deltaTime, int map[], int mapWidth, int mapHeight);
        void SmartEnemy(float deltaTime, int map[], int mapWidth, int mapHeight, float playerX, float playerY);
};

#endif // enemy