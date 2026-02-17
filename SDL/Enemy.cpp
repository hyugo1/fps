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
    hitpoint -= amount;
}

bool Enemy::IsDead() const {
    return hitpoint <= 0;
}

int Enemy::GetHP() const {
    return hitpoint;
}

void Enemy::Update(float deltaTime, std::function<bool(const Entity&, float, float)> collisionFunc, float playerX, float playerY) {
    if (character == horizontalEnemy) {
        HorizontalMove(deltaTime, collisionFunc);
    }
    if (character == verticalEnemy) {
        VerticalMove(deltaTime, collisionFunc);
    }
    if (character == smartEnemy) {
        SmartEnemy(deltaTime, collisionFunc, playerX, playerY);
    }
}

void Enemy::HorizontalMove(float deltaTime, std::function<bool(const Entity&, float, float)> collisionFunc) {
    float speed = 125.0f; // pixels per second

    float nextX = body.x + directionX * speed * deltaTime;
    if (collisionFunc(body, nextX, body.y)) {
        directionX = -directionX; // reverse
    } else {
        body.x = nextX;
    }
}

void Enemy::VerticalMove(float deltaTime, std::function<bool(const Entity&, float, float)> collisionFunc) {
    float speed = 125.0f; // pixels per second

    float nextY = body.y + directionY * speed * deltaTime;
    if (collisionFunc(body, body.x, nextY)) {
        directionY = -directionY; // reverse
    } else {
        body.y = nextY;
    }
}

void Enemy::SmartEnemy(float deltaTime, std::function<bool(const Entity&, float, float)> collisionFunc, float playerX, float playerY) {
    float speed = 125.0f; 
    float dx = playerX - body.x;
    float dy = playerY - body.y;
    float distance = sqrt(dx*dx + dy*dy);
    if(distance < 200) { 
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
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &enemyRect);
}
