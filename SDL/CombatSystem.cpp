#include "CombatSystem.h"

#include <algorithm>
#include <SDL2/SDL.h>

namespace CombatSystem {

bool AABB(const Entity& a, const Entity& b) {
    return (a.x < b.x + b.width &&
            a.x + a.width > b.x &&
            a.y < b.y + b.height &&
            a.y + a.height > b.y);
}

void UpdateEnemy(
    float deltaTime,
    std::vector<Enemy>& enemies,
    const Entity& player,
    int playerMeleeDamage,
    const std::function<bool(const Entity&, float, float)>& collisionFunc,
    bool meleePressed,
    bool& levelComplete) {

    if (meleePressed) {
        for (auto& enemy : enemies) {
            if (!enemy.IsDead() && AABB(player, enemy.getBody())) {
                enemy.TakeDamage(playerMeleeDamage);
            }
        }
    }

    Enemy::UpdateContext updateContext{deltaTime, player.x, player.y, collisionFunc};
    for (auto& enemy : enemies) {
        enemy.Update(updateContext);
    }

    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](Enemy& enemy) { return enemy.IsRemovable(); }),
        enemies.end()
    );
    // Check if all enemies are defeated after updating so its on the same frame as the last enemy dies, not the next frame
    if (enemies.empty()) {
        levelComplete = true;
    }
}

void UpdatePlayerCollision(
    float deltaTime,
    float dx,
    float dy,
    Entity& player,
    std::vector<Enemy>& enemies,
    float& playerInvulnTimer,
    float playerSpeed,
    int& playerHP,
    const std::function<bool(const Entity&, float, float)>& collisionFunc
) {
    float nextX = player.x + dx * playerSpeed * deltaTime;
    float nextY = player.y + dy * playerSpeed * deltaTime;

    if (!collisionFunc(player, nextX, player.y)) {
        player.x = nextX;
    }

    if (!collisionFunc(player, player.x, nextY)) {
        player.y = nextY;
    }

    for (auto& enemy : enemies) {
        if (AABB(player, enemy.getBody())) {
            if (!enemy.IsDead() && playerInvulnTimer <= 0.0f) {
                playerHP -= 10;
                playerInvulnTimer = 1.0f;
            }
        }
    }
}

void DetectMouseClick(
    const Entity& player,
    std::vector<Weapon>& playerWeapons,
    int currentWeaponIndex,
    std::vector<Bullet>& bullets,
    float worldMouseX,
    float worldMouseY,
    bool firePressed
) {
    if (!firePressed) {
        return;
    }

    if (!playerWeapons.empty()) {
        playerWeapons[currentWeaponIndex].Fire(
            player.x + player.width / 2,
            player.y + player.height / 2,
            worldMouseX,
            worldMouseY,
            bullets
        );
    }
}

void UpdateBullets(
    float deltaTime,
    std::vector<Bullet>& bullets,
    std::vector<Enemy>& enemies,
    const std::function<bool(const Entity&, float, float)>& collisionFunc,
    const std::function<void(float, float, int)>& onWallHit
) {
    for (auto& bullet : bullets) {
        bullet.x += bullet.dx * bullet.speed * deltaTime;
        bullet.y += bullet.dy * bullet.speed * deltaTime;
    }

    for (auto& bullet : bullets) {
        Entity bulletEntity{bullet.x, bullet.y, 5, 5};

        for (auto& enemy : enemies) {
            if (!enemy.IsDead() && AABB(bulletEntity, enemy.getBody())) {
                enemy.TakeDamage(bullet.damage);
                bullet.toDelete = true;
                break; // bullet only hits one enemy
            }
        }
    }
    
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return b.toDelete; }),
        bullets.end()
    );
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [&](const Bullet& bullet) {
                Entity bulletEntity{bullet.x, bullet.y, 5, 5};
                if (collisionFunc(bulletEntity, bullet.x, bullet.y)) {
                    // Call wall hit effect with bullet position and damage
                    if (onWallHit) {
                        onWallHit(bullet.x + bulletEntity.width * 0.5f,
                                bullet.y + bulletEntity.height * 0.5f,
                                bullet.damage);
                            }
                    return true;
                }
                return false;
            }),
        bullets.end()
    );
}
}