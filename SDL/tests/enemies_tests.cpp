#include "Enemy.h"
#include "Weapon.h"
#include "CombatSystem.h"

#include <cmath>
#include <iostream>
#include <vector>

namespace {
int failures = 0;

void Expect(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "[FAIL] " << message << std::endl;
        failures++;
    }
}

void ExpectNear(float actual, float expected, float tolerance, const char* message) {
    if (std::fabs(actual - expected) > tolerance) {
        std::cerr << "[FAIL] " << message << std::endl;
        failures++;
    }
}


void TestVerticalEnemyMovement() {
    std::vector<Enemy> enemies;
    Entity player{100.0f, 100.0f, 32.0f, 32.0f};
    
    // spawn vertical enemy
    enemies.push_back(Enemy(120.0f, 100.0f, Enemy::verticalEnemy, 1, 1.0f));
    Expect(enemies[0].GetX() == 120.0f, "Enemy should start at 120.0f X");
    Expect(enemies[0].GetY() == 100.0f, "Enemy should start at 100.0f Y");

    // simulate movement for 1 second
    float deltaTime = 1.0f;
    Enemy::CollisionFunc collisionFunc = [](const Entity& entity, float nextX, float nextY) {
        // simple collision function that treats the world bounds as walls
        if (nextX < 0 || nextX + entity.width > 800 || nextY < 0 || nextY + entity.height > 600) {
            return true; // collision with wall
        }
        return false; // no collision
    };
    Enemy::UpdateContext context{deltaTime, player.x, player.y, collisionFunc};
    enemies[0].Update(context);

    Expect(enemies[0].GetX() == 120.0f, "Vertical enemy should not move horizontally");
    Expect(enemies[0].GetY() != 100.0f, "Vertical enemy should move vertically");
}


void TestHorizontalEnemyMovement() {
    std::vector<Enemy> enemies;
    Entity player{100.0f, 100.0f, 32.0f, 32.0f};
    enemies.push_back(Enemy(120.0f, 100.0f, Enemy::horizontalEnemy, 1, 1.0f));
    Expect(enemies[0].GetX() == 120.0f, "Enemy should start at 120.0f X");
    Expect(enemies[0].GetY() == 100.0f, "Enemy should start at 100.0f Y");

    // simulate movement for 1 second
    float deltaTime = 1.0f;
    Enemy::CollisionFunc collisionFunc = [](const Entity& entity, float nextX, float nextY) {
        // simple collision function that treats the world bounds as walls
        if (nextX < 0 || nextX + entity.width > 800 || nextY < 0 || nextY + entity.height > 600) {
            return true; // collision with wall
        }
        return false; // no collision
    };
    Enemy::UpdateContext context{deltaTime, player.x, player.y, collisionFunc};
    enemies[0].Update(context);

    Expect(enemies[0].GetX() != 120.0f, "Horizontal enemy should move horizontally");
    Expect(enemies[0].GetY() == 100.0f, "Horizontal enemy should not move vertically");
}

void TestEnemySpawns() {
    std::vector<Enemy> enemies;
    enemies.push_back(Enemy(50.0f, 70.0f, Enemy::horizontalEnemy, 1, 1.0f));
    Expect(enemies.size() == 1, "Should have one enemy");
    Expect(enemies[0].GetHP() == 100, "Should have 100 health by default");
    ExpectNear(enemies[0].GetSpeed(), 100.0f, 0.001f, "Should have 100 speed by default");
    Expect(enemies[0].GetX() == 50.0f, "Enemy should be spawning at 50.0f X");
    Expect(enemies[0].GetY() == 70.0f, "Enemy should be spawning at 70.0f Y");
    
    // add second enemy
    enemies.push_back(Enemy(70.0f, 50.0f, Enemy::verticalEnemy, 1, 1.0f));
    Expect(enemies.size() == 2, "Should have two enemies");
    Expect(enemies[1].GetHP() == 100, "Should have 100 health by default");
    ExpectNear(enemies[1].GetSpeed(), 100.0f, 0.001f, "Should have 100 speed by default");
    Expect(enemies[1].GetX() == 70.0f, "Enemy should be spawning at 70.0f X");
    Expect(enemies[1].GetY() == 50.0f, "Enemy should be spawning at 50.0f Y");

    //add third enemy
    enemies.push_back(Enemy(50.0f, 50.0f, Enemy::verticalEnemy, 1, 1.0f));
    Expect(enemies.size() == 3, "Should have three enemies");
    Expect(enemies[2].GetHP() == 100, "Should have 100 health by default");
    ExpectNear(enemies[2].GetSpeed(), 100.0f, 0.001f, "Should have 100 speed by default");
    Expect(enemies[2].GetX() == 50.0f, "Enemy should be spawning at 50.0f X");
    Expect(enemies[2].GetY() == 50.0f, "Enemy should be spawning at 50.0f Y");

}

void TestEnemyLevelUp() {
    // baseSpeed = 100, lvl = 0, difficultyMultiplier = 1.0f
    // baseSpeed = 100, lvl = 1, difficultyMultiplier = 1.0f
    // baseSpeed = 100, lvl = 2, difficultyMultiplier = 1.0f -> speed = 102
    // speed = (baseSpeed + (level - 1) * 2.0f) * difficultyMultiplier;
    // maxHealth = baseHealth + (level - 1) * 2 * difficultyMultiplier;
    std::vector<Enemy> enemies;
    //lvl 2
    enemies.push_back(Enemy(50.0f, 50.0f, Enemy::horizontalEnemy, 2, 1.0f));
    Expect(enemies[0].GetHP() == 102, "should have 102 health on level 2");
    ExpectNear(enemies[0].GetSpeed(), 102.0f, 0.001f, "should have 102 speed on level 2");
    
    //lvl 3
    enemies.push_back(Enemy(100.0f, 100.0f, Enemy::horizontalEnemy, 3, 1.0f));
    Expect(enemies[1].GetHP() == 104, "should have 104 health on level 3");
    ExpectNear(enemies[1].GetSpeed(), 104.0f, 0.001f, "should have 104 speed on level 3");
}

void TestEnemyDifficulty() {
    std::vector<Enemy> enemies;
    //easy
    enemies.push_back(Enemy(50.0f, 50.0f, Enemy::horizontalEnemy, 1, 0.8f));
    Expect(enemies[0].GetHP() == 100, "level 1 should keep base 100 health on easy");
    ExpectNear(enemies[0].GetSpeed(), 80.0f, 0.001f, "should have 80 speed on easy");
    //hard
    enemies.push_back(Enemy(100.0f, 100.0f, Enemy::horizontalEnemy, 1, 1.2f));
    Expect(enemies[1].GetHP() == 100, "level 1 should keep base 100 health on hard");
    ExpectNear(enemies[1].GetSpeed(), 120.0f, 0.001f, "should have 120 speed on hard");
}


void TestEnemyTakesDamageFromBullets() {
    std::vector<Enemy> enemies;
    enemies.push_back(Enemy(50.0f, 50.0f, Enemy::horizontalEnemy, 1, 1.0f));

    Weapon pistol(Weapon::PISTOL);
    std::vector<Bullet> bullets;

    int initialHP = enemies[0].GetHP();
    Expect(initialHP == 100, "Enemy should start with 100 HP");

    pistol.Fire(0.0f, 50.0f, 100.0f, 50.0f, bullets);
    Expect(!bullets.empty(), "Pistol should spawn a bullet");

    CombatSystem::UpdateBullets(
        0.2f, // move bullet enough to reach enemy
        bullets,
        enemies,
        [](const Entity&, float, float) { return false; } // no wall collision in test
    );

    int hpAfter = enemies[0].GetHP();
    Expect(hpAfter < initialHP, "Enemy HP should decrease after bullet collision");
    Expect(hpAfter == 0, "Pistol damage should kill this level-1 enemy in one hit");
}

// void TestGameOverScoreReset() {
// }

void TestScoreIncrements() {
    std::vector<Enemy> enemies;
    enemies.push_back(Enemy(100.0f, 100.0f, Enemy::horizontalEnemy, 1, 1.0f));

    Entity player{100.0f, 100.0f, 32.0f, 32.0f};
    int playerMeleeDamage = 100;

    int score = 0;
    int currentLevel = 1;
    int enemiesBefore = (int)enemies.size();
    bool levelComplete = false;
    float deltaTime = 0.3f;

    CombatSystem::UpdateEnemy(
        deltaTime,
        enemies,
        player,
        playerMeleeDamage,
        [](const Entity&, float, float) { return false; },
        true,
        levelComplete
    );

    int enemiesAfter = (int)enemies.size();
    int kills = enemiesBefore - enemiesAfter;
    if (kills > 0) {
        score += kills * (100 * currentLevel);
    }

    Expect(kills == 1, "One enemy should be removed after lethal melee hit");
    Expect(score == 100, "Score should increase by exactly 100 for one kill at level 1");
    Expect(levelComplete, "Level should complete when all enemies are removed");
}
}

int main() {
    TestVerticalEnemyMovement();
    TestHorizontalEnemyMovement();
    TestEnemySpawns();
    TestEnemyLevelUp();
    TestEnemyDifficulty();
    TestEnemyTakesDamageFromBullets();
    TestScoreIncrements();
    if (failures == 0) {
        std::cout << "All enemy tests passed." << std::endl;
        return 0;
    }

    std::cerr << failures << " test(s) failed." << std::endl;
    return 1;
}
