#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Enemy.h"
#include <vector>
#include <memory>
#include <chrono>

class Game {
private:
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    bool running;
    std::chrono::steady_clock::time_point lastFrameTime;
    int score;
    int wave;

    void spawnEnemies(int count);
    void updateEnemies(float deltaTime);
    void checkCollisions();
    float calculateDeltaTime();

public:
    Game();
    ~Game();

    void initialize();
    void update();
    void run();
    void cleanup();

    bool isRunning() const;
    void stop();
    Player* getPlayer();
    int getScore() const;
    int getWave() const;
};

#endif // GAME_H
