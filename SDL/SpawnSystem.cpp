#include "SpawnSystem.h"

#include <cmath>
#include <cstdlib>
#include "Config.h"

namespace SpawnSystem {

template<typename ItemT>
static void SpawnItems(
    int count,
    std::vector<ItemT>& items,
    const Entity& player,
    int mapWidth,
    int mapHeight,
    int tileSize,
    const std::function<bool(const Entity&, float, float)>& collisionFunc
) {
    const float MIN_DISTANCE = 100.0f;

    for (int i = 0; i < count; i++) {
        float spawnX, spawnY;
        bool collidesWithWall;
        float distance;

        do {
            spawnX = rand() % (mapWidth * tileSize);
            spawnY = rand() % (mapHeight * tileSize);

            Entity temp;
            temp.x = spawnX;
            temp.y = spawnY;
            temp.width = PLAYER_SIZE;
            temp.height = PLAYER_SIZE;

            collidesWithWall = collisionFunc(temp, spawnX, spawnY);

            float dx = spawnX - player.x;
            float dy = spawnY - player.y;
            distance = std::sqrt(dx * dx + dy * dy);
        } while (collidesWithWall || distance < MIN_DISTANCE);

        ItemT item;
        item.x = spawnX;
        item.y = spawnY;
        item.width = PLAYER_SIZE / 2;
        item.height = PLAYER_SIZE / 2;
        items.push_back(item);
    }
}

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
) {
    const float MIN_DISTANCE = 200.0f;
    for(int i = 0; i < count; i++) {
        float spawnX, spawnY;
        bool collidesWithWall;
        float distance;
        do {
            spawnX = rand() % (mapWidth * tileSize);
            spawnY = rand() % (mapHeight * tileSize);
            // Check wall collision
            Entity temp; // create a temporary entity for collision checking
            temp.x = spawnX;
            temp.y = spawnY;
            temp.width = PLAYER_SIZE;
            temp.height = PLAYER_SIZE;
            collidesWithWall = collisionFunc(temp, spawnX, spawnY);
            // Check distance from player
            float dx = spawnX - player.x;
            float dy = spawnY - player.y;
            distance = std::sqrt(dx*dx + dy*dy);
        } while(collidesWithWall || distance < MIN_DISTANCE); // ensure enemies don't spawn too close to the player or inside walls
        Enemy::EnemyType type =
            static_cast<Enemy::EnemyType>(rand() % 3);
        enemies.push_back(Enemy(spawnX, spawnY, type, currentLevel, difficultyMultiplier));
    }
}

void SpawnHealthItems(
    int count,
    std::vector<HealthItem>& healthItems,
    const Entity& player,
    int mapWidth,
    int mapHeight,
    int tileSize,
    const std::function<bool(const Entity&, float, float)>& collisionFunc
) {
    SpawnItems(count, healthItems, player, mapWidth, mapHeight, tileSize, collisionFunc);
}

void SpawnSpeedItems(
    int count,
    std::vector<SpeedItem>& speedItems,
    const Entity& player,
    int mapWidth,
    int mapHeight,
    int tileSize,
    const std::function<bool(const Entity&, float, float)>& collisionFunc
) {
    SpawnItems(count, speedItems, player, mapWidth, mapHeight, tileSize, collisionFunc);
}

void SpawnWeaponItems(
    int count,
    std::vector<WeaponItem>& weaponItems,
    const Entity& player,
    int currentLevel,
    int mapWidth,
    int mapHeight,
    int tileSize,
    const std::function<bool(const Entity&, float, float)>& collisionFunc
) {
    const float MIN_DISTANCE = 100.0f;

    for (int i = 0; i < count; i++) {
        float spawnX, spawnY;
        bool collidesWithWall;
        float distance;

        do {
            spawnX = rand() % (mapWidth * tileSize);
            spawnY = rand() % (mapHeight * tileSize);

            Entity temp;
            temp.x = spawnX;
            temp.y = spawnY;
            temp.width = PLAYER_SIZE;
            temp.height = PLAYER_SIZE;

            collidesWithWall = collisionFunc(temp, spawnX, spawnY);

            float dx = spawnX - player.x;
            float dy = spawnY - player.y;
            distance = std::sqrt(dx*dx + dy*dy);

        } while(collidesWithWall || distance < MIN_DISTANCE);

        WeaponItem item;
        item.x = spawnX;
        item.y = spawnY;
        item.width = PLAYER_SIZE / 2;
        item.height = PLAYER_SIZE / 2;
    
        if (currentLevel == 2) {
            item.type = Weapon::RIFLE;
        } else if (currentLevel == 3) {
            item.type = Weapon::SHOTGUN;
        } else {
            item.type = Weapon::MACHINEGUN;
        }

        weaponItems.push_back(item);
    }
}
}