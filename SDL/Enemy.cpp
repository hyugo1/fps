//Enemy.cpp

#include <stdio.h>
#include <cmath>
#include <vector>
#include <string>
#include "Enemy.h"
#include "Entity.h"
#include "Config.h"
#include <SDL2/SDL_image.h>

namespace {
SDL_Texture* LoadTextureWithFallback(SDL_Renderer* renderer, const std::string& relativePath) {
    std::vector<std::string> candidatePaths = {
        relativePath,
        "SDL/" + relativePath,
        "../" + relativePath,
        "../../SDL/" + relativePath
    };

    char* basePathRaw = SDL_GetBasePath();
    if (basePathRaw) {
        std::string basePath(basePathRaw);
        candidatePaths.push_back(basePath + relativePath);
        candidatePaths.push_back(basePath + "../" + relativePath);
        candidatePaths.push_back(basePath + "../SDL/" + relativePath);
        SDL_free(basePathRaw);
    }

    for (const std::string& path : candidatePaths) {
        SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
        if (texture) {
            return texture;
        }
    }

    return nullptr;
}
}

// ---------------- Constructor ----------------
Enemy::Enemy(float startX, float startY, EnemyType type, int level, float difficultyMultiplier) {
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
    speed = (baseSpeed + (level - 1) * 2.0f) * difficultyMultiplier;
    maxHealth = baseHealth + (level - 1) * 2 * difficultyMultiplier;
    health = maxHealth;
    isDying = false;
    deathDuration = 0.25f;
    deathTimer = 0.0f;
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
    if (isDying || health <= 0) {
        return;
    }

    health -= amount;
    // if health drops to 0 or below, start death animation
    if (health <= 0) {
        health = 0;
        isDying = true;
        deathTimer = deathDuration;
    }
}

bool Enemy::IsDead() const {
    return health <= 0;
}

// Returns true if enemy is in death animation, false if still alive or already removed
bool Enemy::IsDying() const {
    return isDying;
}

// Returns true if enemy should be removed from game (after death animation finishes)
bool Enemy::IsRemovable() const {
    return isDying && deathTimer <= 0.0f;
}

int Enemy::GetHP() const {
    return health;
}

int Enemy::GetMaxHP() const { 
    return maxHealth; }

void Enemy::Update(float deltaTime, std::function<bool(const Entity&, float, float)> collisionFunc, float playerX, float playerY) {    
    if (isDying) {
        deathTimer -= deltaTime;
        if (deathTimer < 0.0f) {
            deathTimer = 0.0f;
        }
        return;
    }

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
    // distance check to prevent division by zero and only move if player is within maxdistance 
    if(distance > 0.0f && distance < maxdistance) { 
        dx /= distance;
        dy /= distance;
        float nextX = body.x + dx * speed * deltaTime;
        float nextY = body.y + dy * speed * deltaTime;
        if(!collisionFunc(body, nextX, body.y)) body.x = nextX;
        if(!collisionFunc(body, body.x, nextY)) body.y = nextY;
    }
}

void Enemy::Render(float cameraX, float cameraY, SDL_Renderer* renderer) {
    static SDL_Texture* horizontalTexture = nullptr;
    static SDL_Texture* verticalTexture = nullptr;
    static SDL_Texture* smartTexture = nullptr;
    static bool texturesLoaded = false;

    if (!texturesLoaded) {
        horizontalTexture = LoadTextureWithFallback(renderer, "sprites/enemy.png");
        verticalTexture = LoadTextureWithFallback(renderer, "sprites/enemy2.png");
        smartTexture = LoadTextureWithFallback(renderer, "sprites/enemy3.png");
        texturesLoaded = true;
    }

    // If isDying, render death animation instead of normal enemy
    if (isDying) {
        float progress = 1.0f - (deathTimer / deathDuration);
        if (progress < 0.0f) progress = 0.0f;
        if (progress > 1.0f) progress = 1.0f;

        int expandedSize = (int)(body.width * (1.0f + 0.7f * progress));
        int centerX = (int)(body.x + body.width * 0.5f - cameraX);
        int centerY = (int)(body.y + body.height * 0.5f - cameraY);
        SDL_Rect deathRect = {
            centerX - expandedSize / 2,
            centerY - expandedSize / 2,
            expandedSize,
            expandedSize
        };

        Uint8 baseR = 255;
        Uint8 baseG = 100;
        Uint8 baseB = 30;
        if (character == smartEnemy) {
            baseR = 194; baseG = 45; baseB = 252;
        } else if (character == horizontalEnemy) {
            baseR = 90; baseG = 252; baseB = 45;
        } else if (character == verticalEnemy) {
            baseR = 49; baseG = 90; baseB = 255;
        }

        Uint8 alpha = (Uint8)(255.0f * (1.0f - progress));
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, baseR, baseG, baseB, alpha);
        SDL_RenderFillRect(renderer, &deathRect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        return;
    }

    //draw enemy
    SDL_Rect enemyRect = { 
        (int)(body.x - cameraX), 
        (int)(body.y - cameraY), 
        (int)body.width, (int)body.height
    };

    SDL_Texture* currentEnemyTexture = nullptr;
    Uint8 baseR = 255, baseG = 255, baseB = 255;

    if (character == horizontalEnemy) {
        currentEnemyTexture = horizontalTexture;
        baseR = 90; baseG = 252; baseB = 45;
    } else if (character == verticalEnemy) {
        currentEnemyTexture = verticalTexture;
        baseR = 49; baseG = 90; baseB = 255;
    } else if (character == smartEnemy) {
        currentEnemyTexture = smartTexture;
        baseR = 194; baseG = 45; baseB = 252;
    }

    float healthPercent = (float)health / maxHealth;

    Uint8 r = baseR * healthPercent;
    Uint8 g = baseG * healthPercent;
    Uint8 b = baseB * healthPercent;

    if (currentEnemyTexture) {
        SDL_SetTextureColorMod(currentEnemyTexture, 255, 255, 255);
        SDL_SetTextureAlphaMod(currentEnemyTexture, 255);
        SDL_SetTextureBlendMode(currentEnemyTexture, SDL_BLENDMODE_BLEND);
        SDL_RenderCopy(renderer, currentEnemyTexture, nullptr, &enemyRect);
    } else {
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderFillRect(renderer, &enemyRect);
    }
}