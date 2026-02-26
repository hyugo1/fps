#include "Enemy.h"
#include "Weapon.h"
#include "CombatSystem.h"

#include <iostream>
#include <vector>
#include <algorithm>

namespace {
int failures = 0;

void Expect(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "[FAIL] " << message << std::endl;
        failures++;
    }
}

void testPlayerStats() {
    Entity player{100.0f, 100.0f, 32.0f, 32.0f};
    int playerHP = 100;
    int playerMeleeDamage = 10;

    Expect(playerHP == 100, "Player should start with 100 HP");
    Expect(playerMeleeDamage == 10, "Player melee damage should be 10");
}


void testPlayerCollisionWithEnemy() {
    std::vector<Enemy> enemies;
    Entity player{100.0f, 100.0f, 32.0f, 32.0f};
    enemies.push_back(Enemy(100.0f, 100.0f, Enemy::horizontalEnemy, 1, 1.0f));

    float deltaTime = 0.016f;
    float playerInvulnTimer = 0.0f;
    float playerSpeed = 100.0f;
    int playerHP = 100;

    CombatSystem::UpdatePlayerCollision(
        deltaTime,
        0.0f, 0.0f,                // no movement needed
        player,
        enemies,
        playerInvulnTimer,
        playerSpeed,
        playerHP,
        [](const Entity&, float, float) { return false; }
    );

    Expect(playerHP == 90, "Player should lose 10 HP on enemy collision");
    Expect(playerInvulnTimer > 0.0f, "Player invulnerability should start after hit");

    // check if invulnerability prevents further damage
    CombatSystem::UpdatePlayerCollision(
        deltaTime,
        0.0f, 0.0f,                // no movement needed
        player,
        enemies,
        playerInvulnTimer,
        playerSpeed,
        playerHP,
        [](const Entity&, float, float) { return false; }
    );

    Expect(playerHP == 90, "Player should not take damage while invulnerable");
    Expect(playerInvulnTimer > 0.0f, "Player invulnerability should still be active");


    playerInvulnTimer = std::max(0.0f, playerInvulnTimer - 1.0f);
    Expect(playerInvulnTimer == 0.0f, "Player invulnerability should expire after timer runs out");
    CombatSystem::UpdatePlayerCollision(
        deltaTime,
        0.0f, 0.0f,                // no movement needed
        player,
        enemies,
        playerInvulnTimer,
        playerSpeed,
        playerHP,
        [](const Entity&, float, float) { return false; }
     );
    Expect(playerHP == 80, "Player should take damage again after invulnerability expires");
    Expect(playerInvulnTimer > 0.0f, "Player invulnerability should restart after taking damage again");
}
}

int main() {
    testPlayerStats();
    testPlayerCollisionWithEnemy();
    if (failures == 0) {
        std::cout << "All player tests passed." << std::endl;
        return 0;
    }

    std::cerr << failures << " test(s) failed." << std::endl;
    return 1;
}
