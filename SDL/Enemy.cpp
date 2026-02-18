//Enemy.cpp

#include <stdio.h>
#include <cmath>
#include <vector>
#include "Enemy.h"
#include "Entity.h"
#include "Config.h"

// ---------------- Constructor ----------------
Enemy::Enemy(float startX, float startY, EnemyType type, int level) {
    body.x = startX;
    body.y = startY;
    body.width = PLAYER_SIZE;
    body.height = PLAYER_SIZE;
    tileSize = TILE_SIZE;
    directionX = 1;
    directionY = 1;
    character = type;
    float baseSpeed = 100.0f;
    int baseHealth = 100;
    speed = baseSpeed + (level - 1) * 2.0f;
    maxHealth = baseHealth + (level - 1) * 2;
    health = maxHealth;
    maxdistance = 200.0f + (level - 1) * 5.0f;
}

float Enemy::GetX() const {
    return body.x;
}

float Enemy::GetY() const {
    return body.y;
}

const Entity& Enemy::getBody() const {
    return body;
}

void Enemy::TakeDamage(int amount) {
    health -= amount;
}

bool Enemy::IsDead() const {
    return health <= 0;
}

int Enemy::GetHP() const {
    return health;
}

int Enemy::GetMaxHP() const { 
    return maxHealth; }

void Enemy::Update(float deltaTime, std::function<bool(const Entity&, float, float)> collisionFunc, float playerX, float playerY)
{
    switch (character)
    {
        case horizontalEnemy:
            HorizontalMove(deltaTime, collisionFunc);
            break;

        case verticalEnemy:
            VerticalMove(deltaTime, collisionFunc);
            break;

        case smartEnemy:
            SmartEnemy(deltaTime, collisionFunc, playerX, playerY);
            break;
    }
}

void Enemy::HorizontalMove(float deltaTime, std::function<bool(const Entity&, float, float)> collisionFunc) {
    float nextX = body.x + directionX * speed * deltaTime;
    if (collisionFunc(body, nextX, body.y)) {
        directionX = -directionX; // reverse
    } else {
        body.x = nextX;
    }
}

void Enemy::VerticalMove(float deltaTime, std::function<bool(const Entity&, float, float)> collisionFunc) {
    float nextY = body.y + directionY * speed * deltaTime;
    if (collisionFunc(body, body.x, nextY)) {
        directionY = -directionY; // reverse
    } else {
        body.y = nextY;
    }
}

void Enemy::SmartEnemy(float deltaTime, std::function<bool(const Entity&, float, float)> collisionFunc, float playerX, float playerY) {
    float dx = playerX - body.x;
    float dy = playerY - body.y;
    float distance = sqrt(dx*dx + dy*dy);
    if(distance < maxdistance) { 
        dx /= distance;
        dy /= distance;
        float nextX = body.x + dx * speed * deltaTime;
        float nextY = body.y + dy * speed * deltaTime;
        if(!collisionFunc(body, nextX, body.y)) body.x = nextX;
        if(!collisionFunc(body, body.x, nextY)) body.y = nextY;
    }
}

void Enemy::Render(float cameraX, float cameraY, SDL_Renderer* renderer) {
    //draw enemy
    SDL_Rect enemyRect = { 
        (int)(body.x - cameraX), 
        (int)(body.y - cameraY), 
        (int)body.width, (int)body.height
    };

    Uint8 baseR, baseG, baseB;

    if (character == horizontalEnemy) {
        baseR = 255; baseG = 0; baseB = 0; // red
    } else if (character == verticalEnemy) {
        baseR = 255; baseG = 165; baseB = 0; // orange
    } else if (character == smartEnemy) {
        baseR = 0; baseG = 200; baseB = 255; // cyan
    }

    float healthPercent = (float)health / maxHealth;

    Uint8 r = baseR * healthPercent;
    Uint8 g = baseG * healthPercent;
    Uint8 b = baseB * healthPercent;

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    SDL_RenderFillRect(renderer, &enemyRect);
}