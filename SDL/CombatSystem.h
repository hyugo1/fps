#ifndef COMBAT_SYSTEM_H
#define COMBAT_SYSTEM_H

#include <vector>
#include <functional>
#include "Entity.h"
#include "Enemy.h"
#include "Weapon.h"

namespace CombatSystem {

bool AABB(const Entity& a, const Entity& b);

void UpdateEnemy(
    float deltaTime,
    std::vector<Enemy>& enemies,
    const Entity& player,
    int playerMeleeDamage,
    const std::function<bool(const Entity&, float, float)>& collisionFunc,
    bool meleePressed,
    bool& levelComplete
);

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
);

void DetectMouseClick(
    int cameraX,
    int cameraY,
    const Entity& player,
    std::vector<Weapon>& playerWeapons,
    int currentWeaponIndex,
    std::vector<Bullet>& bullets
);

void UpdateBullets(
    float deltaTime,
    std::vector<Bullet>& bullets,
    std::vector<Enemy>& enemies,
    const std::function<bool(const Entity&, float, float)>& collisionFunc
);

}

#endif