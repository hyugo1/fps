//Enemy.cpp
#include "Game.h"
#include <stdio.h>
#include <cmath>
#include <vector>
#include "Enemy.h"

// ---------------- Constructor ----------------
Enemy::Enemy() {
    enemyX = 100;
    enemyY = 100;
    hitpoint = 100;
    tileSize = 50;
    directionX = 1; 
}

float Enemy::getX() const {
    return enemyX;
}

float Enemy::getY() const {
    return enemyY;
}

void Enemy::getEnemyPosition(float &x, float &y) {
    x = enemyX;
    y = enemyY;
}

bool Enemy::detectCollision(float x, float y, int map[], int mapWidth, int mapHeight) {
    int enemySize = 50;
    int leftTile   = (int)(x / tileSize);
    int rightTile  = (int)((x + enemySize - 1) / tileSize);
    int topTile    = (int)(y / tileSize);
    int bottomTile = (int)((y + enemySize - 1) / tileSize);

    // Check each corner?
    for(int tileY = topTile; tileY <= bottomTile; tileY++) {
        for(int tileX = leftTile; tileX <= rightTile; tileX++) {
            if(tileX >= 0 && tileX < mapWidth && tileY >= 0 && tileY < mapHeight) {
                if(map[tileY * mapWidth + tileX] == 1) {
                    return true;
                }
            }
        }
    }
    return false;
}

void Enemy::Update(float deltaTime, int map[], int mapWidth, int mapHeight) {
    //direction vector
    //move right initially
    // float dx = 1; 
    // float dy = 0;
    
    float speed = 100.0f; // pixels per second

    float nextX = enemyX + directionX * speed * deltaTime;
    if (detectCollision(nextX, enemyY, map, mapWidth, mapHeight)) {
        directionX = -directionX; // reverse
    } else {
        enemyX = nextX;
    }
}

void Enemy::Render(float cameraX, float cameraY, SDL_Renderer* renderer) {
    //draw enemy
    SDL_Rect enemyRect = { 
        (int)(enemyX - cameraX), 
        (int)(enemyY - cameraY), 
        50, 50 
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &enemyRect);
}
