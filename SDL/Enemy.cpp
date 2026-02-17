//Enemy.cpp

#include <stdio.h>
#include <cmath>
#include <vector>
#include "Enemy.h"
#include "Entity.h"
#include "Config.h"

// ---------------- Constructor ----------------
Enemy::Enemy(float startX, float startY, EnemyType type) {
    body.x = startX;
    body.y = startY;
    body.width = PLAYER_SIZE;
    body.height = PLAYER_SIZE;
    hitpoint = 100;
    tileSize = TILE_SIZE;
    directionX = 1;
    directionY = 1;
    character = type;
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

void Enemy::TakeDamage(int amount) {
    hitpoint -= amount;
}

bool Enemy::IsDead() const {
    return hitpoint <= 0;
}

int Enemy::GetHP() const {
    return hitpoint;
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

void Enemy::Update(float deltaTime, int map[], int mapWidth, int mapHeight, float playerX, float playerY) {
    if (character == horizontalEnemy) {
        HorizontalMove(deltaTime, map, mapWidth, mapHeight);
    }
    if (character == verticalEnemy) {
        VerticalMove(deltaTime, map, mapWidth, mapHeight);
    }
    if (character == smartEnemy) {
        SmartEnemy(deltaTime, map, mapWidth, mapHeight, playerX, playerY);
    }
}

void Enemy::HorizontalMove(float deltaTime, int map[], int mapWidth, int mapHeight) {
    float speed = 125.0f; // pixels per second

    float nextX = body.x + directionX * speed * deltaTime;
    if (detectCollision(nextX, body.y, map, mapWidth, mapHeight)) {
        directionX = -directionX; // reverse
    } else {
        body.x = nextX;
    }
}

void Enemy::VerticalMove(float deltaTime, int map[], int mapWidth, int mapHeight) {
    float speed = 125.0f; // pixels per second

    float nextY = body.y + directionY * speed * deltaTime;
    if (detectCollision(body.x, nextY, map, mapWidth, mapHeight)) {
        directionY = -directionY; // reverse
    } else {
        body.y = nextY;
    }
}

void Enemy::SmartEnemy(float deltaTime, int map[], int mapWidth, int mapHeight, float playerX, float playerY) {
    float speed = 125.0f; 
    float dx = playerX - body.x;
    float dy = playerY - body.y;
    float distance = sqrt(dx*dx + dy*dy);
    if(distance < 200) { 
        dx /= distance;
        dy /= distance;
        float nextX = body.x + dx * speed * deltaTime;
        float nextY = body.y + dy * speed * deltaTime;
        if(!detectCollision(nextX, body.y, map, mapWidth, mapHeight)) body.x = nextX;
        if(!detectCollision(body.x, nextY, map, mapWidth, mapHeight)) body.y = nextY;
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
