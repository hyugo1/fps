#ifndef PLAYER_H
#define PLAYER_H

#include "Camera.h"
#include "Vector3.h"
#include <memory>

class Weapon;

class Player {
private:
    Camera camera;
    float health;
    float maxHealth;
    float speed;
    std::shared_ptr<Weapon> currentWeapon;

public:
    Player();
    Player(const Vector3& startPosition, float maxHealth = 100.0f);

    void move(float forward, float right, float deltaTime);
    void rotate(float deltaYaw, float deltaPitch);
    void takeDamage(float damage);
    void heal(float amount);
    void setWeapon(std::shared_ptr<Weapon> weapon);
    void shoot();

    Camera& getCamera();
    const Camera& getCamera() const;
    float getHealth() const;
    float getMaxHealth() const;
    bool isAlive() const;
    Vector3 getPosition() const;
};

#endif // PLAYER_H
