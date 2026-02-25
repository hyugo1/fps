#ifndef SPAWN_SYSTEM_H
#define SPAWN_SYSTEM_H

#include <vector>
#include <functional>
#include "Entity.h"
#include "Enemy.h"
#include "Weapon.h"
#include "Items.h"

namespace SpawnSystem {
    void SpawnEnemies(
        int count,
        std::vector<Enemy>& enemies,
        const Entity& player,
        int currentLevel,
        int mapWidth,
        int mapHeight,
        int tileSize,
        const std::function<bool(const Entity&, float, float)>& collisionFunc,
        float difficultyMultiplier
    );

    void SpawnHealthItems(
        int count,
        std::vector<HealthItem>& healthItems,
        const Entity& player,
        int mapWidth,
        int mapHeight,
        int tileSize,
        const std::function<bool(const Entity&, float, float)>& collisionFunc
    );

    void SpawnSpeedItems(
        int count,
        std::vector<SpeedItem>& speedItems,
        const Entity& player,
        int mapWidth,
        int mapHeight,
        int tileSize,
        const std::function<bool(const Entity&, float, float)>& collisionFunc
    );

    void SpawnWeaponItems(
        int count,
        std::vector<WeaponItem>& weaponItems,
        const Entity& player,
        int currentLevel,
        int mapWidth,
        int mapHeight,
        int tileSize,
        const std::function<bool(const Entity&, float, float)>& collisionFunc
    );
}

#endif