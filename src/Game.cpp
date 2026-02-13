#include "Game.h"
#include "Weapon.h"
#include <iostream>
#include <cmath>
#include <random>
#include <thread>

Game::Game() : running(false), score(0), wave(1) {}

Game::~Game() {
    cleanup();
}

void Game::initialize() {
    std::cout << "==================================" << std::endl;
    std::cout << "    FPS Game - Console Edition" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << std::endl;

    // Create player at origin
    player = std::make_unique<Player>(Vector3(0, 0, 0), 100.0f);

    // Give player a weapon
    auto pistol = std::make_shared<Weapon>("Pistol", 25.0f, 12, 2.0f);
    player->setWeapon(pistol);

    // Spawn initial enemies
    spawnEnemies(3);

    running = true;
    lastFrameTime = std::chrono::steady_clock::now();

    std::cout << "Game initialized!" << std::endl;
    std::cout << "Player Health: " << player->getHealth() << std::endl;
    std::cout << "Enemies: " << enemies.size() << std::endl;
    std::cout << std::endl;
}

void Game::spawnEnemies(int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-10.0f, 10.0f);

    for (int i = 0; i < count; ++i) {
        float x = dist(gen);
        float z = dist(gen);
        auto enemy = std::make_unique<Enemy>(Vector3(x, 0, z), 50.0f, 5.0f);
        enemies.push_back(std::move(enemy));
    }
}

void Game::updateEnemies(float deltaTime) {
    if (!player || !player->isAlive()) return;

    Vector3 playerPos = player->getPosition();

    for (auto& enemy : enemies) {
        if (!enemy->isAlive() || !enemy->isActive()) continue;

        // Move enemy towards player
        enemy->moveTowards(playerPos, deltaTime);

        // Check if enemy is close enough to attack
        Vector3 diff = enemy->getPosition() - playerPos;
        float distance = diff.length();

        if (distance < 2.0f) {
            enemy->attack(*player);
        }
    }
}

void Game::checkCollisions() {
    // Placeholder for collision detection
    // Would implement proper collision detection between player and level geometry
}

float Game::calculateDeltaTime() {
    auto currentTime = std::chrono::steady_clock::now();
    auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - lastFrameTime).count() / 1000.0f;
    lastFrameTime = currentTime;
    return deltaTime;
}

void Game::update() {
    if (!running) return;

    float deltaTime = calculateDeltaTime();

    // Update enemies
    updateEnemies(deltaTime);

    // Check collisions
    checkCollisions();

    // Check if all enemies are dead
    bool allDead = true;
    for (const auto& enemy : enemies) {
        if (enemy->isAlive() && enemy->isActive()) {
            allDead = false;
            break;
        }
    }

    // Spawn new wave if all enemies are dead
    if (allDead && enemies.size() > 0) {
        wave++;
        std::cout << "\nWave " << wave << " completed!" << std::endl;
        std::cout << "Spawning new wave..." << std::endl;
        enemies.clear();
        spawnEnemies(3 + wave);
    }

    // Check if player is dead
    if (!player->isAlive()) {
        std::cout << "\nGame Over!" << std::endl;
        std::cout << "Final Score: " << score << std::endl;
        std::cout << "Waves Survived: " << wave - 1 << std::endl;
        running = false;
    }
}



void Game::run() {
    initialize();

    std::cout << "\nGame running..." << std::endl;
    std::cout << "This is a basic simulation of an FPS game." << std::endl;
    std::cout << "Press Ctrl+C to exit." << std::endl;
    std::cout << std::endl;

    // Simulate a few game ticks
    for (int i = 0; i < 10 && running; ++i) {
        update();

        // Print game state
        if (player->isAlive()) {
            std::cout << "Tick " << i + 1 << ": ";
            std::cout << "Player Health: " << player->getHealth() << " | ";
            std::cout << "Active Enemies: ";

            int activeCount = 0;
            for (const auto& enemy : enemies) {
                if (enemy->isAlive() && enemy->isActive()) {
                    activeCount++;
                }
            }
            std::cout << activeCount << std::endl;
        }

        // Small delay to simulate game tick
        std::chrono::milliseconds delay(500);
        std::this_thread::sleep_for(delay);
    }

    std::cout << "\nSimulation complete." << std::endl;
}

void Game::cleanup() {
    enemies.clear();
    player.reset();
}

bool Game::isRunning() const {
    return running;
}

void Game::stop() {
    running = false;
}

Player* Game::getPlayer() {
    return player.get();
}

int Game::getScore() const {
    return score;
}

int Game::getWave() const {
    return wave;
}
