//Enemy.h

#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>
#include "Entity.h"

class Enemy {
    public:
        Enemy();
        void Update(float deltaTime, int map[], int mapWidth, int mapHeight);
        void Render(float cameraX, float cameraY, SDL_Renderer* renderer);
        float getX() const;
        float getY() const;
        const Entity& getBody() const;

    private:
        Entity body;
        float directionX;
        double hitpoint;
        int tileSize;
        bool detectCollision(float x, float y, int map[], int mapWidth, int mapHeight);
};

#endif // enemy