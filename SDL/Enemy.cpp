//Enemy.cpp

#include <stdio.h>
#include <cmath>
#include <vector>
#include "Enemy.h"
#include "Entity.h"
#include "Config.h"

// ---------------- Constructor ----------------
Enemy::Enemy() {
    body.x = 100;
    body.y = 100;
    body.width = PLAYER_SIZE;
    body.height = PLAYER_SIZE;
    hitpoint = 100;
    tileSize = TILE_SIZE;
    directionX = 1; 
}

float Enemy::getX() const {
    return body.x;
}

float Enemy::getY() const {
    return body.y;
}

const Entity& Enemy::getBody() const {
    return body;
}

bool Enemy::detectCollision(float x, float y, int map[], int mapWidth, int mapHeight) {
    int leftTile   = (int)(x / TILE_SIZE);
    int rightTile  = (int)((x + body.width - 1) / TILE_SIZE);
    int topTile    = (int)(y / TILE_SIZE);
    int bottomTile = (int)((y + body.height - 1) / TILE_SIZE);

    for (int tileY = topTile; tileY <= bottomTile; tileY++) {
        for (int tileX = leftTile; tileX <= rightTile; tileX++) {
            if (tileX >= 0 && tileX < mapWidth && tileY >= 0 && tileY < mapHeight) {
                if (map[tileY * mapWidth + tileX] == 1) {
                    return true; // collision
                }
            }
        }
    }
    return false; // no collision
}

void Enemy::Update(float deltaTime, int map[], int mapWidth, int mapHeight) {
    //direction vector
    //move right initially
    // float dx = 1; 
    // float dy = 0;
    
    float speed = 100.0f; // pixels per second

    float nextX = body.x + directionX * speed * deltaTime;
    if (detectCollision(nextX, body.y, map, mapWidth, mapHeight)) {
        directionX = -directionX; // reverse
    } else {
        body.x = nextX;
    }
}

void Enemy::Render(float cameraX, float cameraY, SDL_Renderer* renderer) {
    //draw enemy
    SDL_Rect enemyRect = { 
        (int)(body.x - cameraX), 
        (int)(body.y - cameraY), 
        (int)body.width, (int)body.height
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &enemyRect);
}
