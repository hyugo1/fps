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

SDL_Texture* Enemy::horizontalTexture = nullptr;
SDL_Texture* Enemy::verticalTexture = nullptr;
SDL_Texture* Enemy::smartTexture = nullptr;
bool Enemy::texturesLoaded = false;

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

void Enemy::EnsureTexturesLoaded(SDL_Renderer* renderer) {
    if (texturesLoaded || renderer == nullptr) {
        return;
    }
    horizontalTexture = LoadTextureWithFallback(renderer, "sprites/enemy.png");
    verticalTexture = LoadTextureWithFallback(renderer, "sprites/enemy2.png");
    smartTexture = LoadTextureWithFallback(renderer, "sprites/enemy3.png");
    texturesLoaded = true;
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

bool Enemy::IsDying() const {
    return isDying;
}

bool Enemy::IsRemovable() const {
    return isDying && deathTimer <= 0.0f;
}

int Enemy::GetHP() const {
    return health;
}

int Enemy::GetMaxHP() const { 
    return maxHealth; 
}

float Enemy::GetSpeed() const {
    return speed;
}

bool Enemy::CheckIfDying(const UpdateContext& context) {
    if (isDying) {
        deathTimer -= context.deltaTime;
        if (deathTimer < 0.0f) {
            deathTimer = 0.0f;
        }
        return true;
    }
    return false;
}

void Enemy::UpdateMovementByType(const UpdateContext& context) {
    switch (character) {
        case horizontalEnemy:
            HorizontalMove(context);
            break;
        case verticalEnemy:
            VerticalMove(context);
            break;
        case smartEnemy:
            SmartEnemy(context);
            break;
    }
}

void Enemy::Update(const UpdateContext& context) {
    if (CheckIfDying(context)) {
        return;
    }
    UpdateMovementByType(context);
}

void Enemy::HorizontalMove(const UpdateContext& context) {
    float nextX = body.x + directionX * speed * context.deltaTime;
    if (context.collisionFunc(body, nextX, body.y)) {
        directionX = -directionX; // reverse
    } else {
        body.x = nextX;
    }
}

void Enemy::VerticalMove(const UpdateContext& context) {
    float nextY = body.y + directionY * speed * context.deltaTime;
    if (context.collisionFunc(body, body.x, nextY)) {
        directionY = -directionY; // reverse
    } else {
        body.y = nextY;
    }
}

float Enemy::GetDistanceToPlayer(float dx, float dy) const {
    return sqrt(dx*dx + dy*dy);
}

void Enemy::SmartEnemy(const UpdateContext& context) {
    float dx = context.playerX - body.x;
    float dy = context.playerY - body.y;
    float distance = GetDistanceToPlayer(dx, dy);
    // distance check to prevent division by zero and only move if player is within maxdistance 
    if(distance > 0.0f && distance < maxdistance) { 
        // Normalize the direction vector
        dx /= distance;
        dy /= distance;
        // Move towards player
        float nextX = body.x + dx * speed * context.deltaTime;
        float nextY = body.y + dy * speed * context.deltaTime;
        if(!context.collisionFunc(body, nextX, body.y)) body.x = nextX;
        if(!context.collisionFunc(body, body.x, nextY)) body.y = nextY;
    }
}

void Enemy::Render(float cameraX, float cameraY, SDL_Renderer* renderer) {
    EnsureTexturesLoaded(renderer);
    if (isDying) {
        RenderDeathEffect(cameraX, cameraY, renderer);
    } else {
        RenderAliveEnemy(cameraX, cameraY, renderer);
    }
}

void Enemy::SetEnemyTextureAndColor() {
    if (character == horizontalEnemy) {
        this->currentEnemyTexture = horizontalTexture;
        this->baseR = 90; this->baseG = 252; this->baseB = 45;
    } else if (character == verticalEnemy) {
        this->currentEnemyTexture = verticalTexture;
        this->baseR = 49; this->baseG = 90; this->baseB = 255;
    } else if (character == smartEnemy) {
        this->currentEnemyTexture = smartTexture;
        this->baseR = 194; this->baseG = 45; this->baseB = 252;
    }

}

float Enemy::GetProgress() const {
    float progress = 1.0f - (deathTimer / deathDuration);
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;
    return progress;
}

void Enemy::RenderAliveEnemy(float cameraX, float cameraY, SDL_Renderer* renderer) {
    //draw enemy
    SDL_Rect enemyRect = DrawEnemyRectangle(cameraX, cameraY);
    SetEnemyTextureAndColor();
    float healthPercent = (float)health / maxHealth;
    Uint8 r = this->baseR * healthPercent;
    Uint8 g = this->baseG * healthPercent;
    Uint8 b = this->baseB * healthPercent;
    if (this->currentEnemyTexture) {
        SDL_SetTextureColorMod(this->currentEnemyTexture, r, g, b);
        SDL_SetTextureAlphaMod(this->currentEnemyTexture, 255);
        SDL_SetTextureBlendMode(this->currentEnemyTexture, SDL_BLENDMODE_BLEND);
        SDL_RenderCopy(renderer, this->currentEnemyTexture, nullptr, &enemyRect);
    } else {
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderFillRect(renderer, &enemyRect);
    }
}

void Enemy::RenderDeathEffect(float cameraX, float cameraY, SDL_Renderer* renderer) const {
    float progress = GetProgress();
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

SDL_Rect Enemy::DrawEnemyRectangle(float cameraX, float cameraY) const {
    return {
        (int)(body.x - cameraX), 
        (int)(body.y - cameraY), 
        (int)body.width, (int)body.height
    };
}