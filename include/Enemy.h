#ifndef ENEMY_H
#define ENEMY_H

#include "Vector3.h"

class Enemy {
private:
    Vector3 position;
    float health;
    float maxHealth;
    float speed;
    float damage;
    bool active;

public:
    Enemy();
    Enemy(const Vector3& pos, float maxHealth = 50.0f, float damage = 10.0f);

    void moveTowards(const Vector3& target, float deltaTime);
    void takeDamage(float damage);
    void attack(class Player& player);

    Vector3 getPosition() const;
    void setPosition(const Vector3& pos);
    float getHealth() const;
    bool isAlive() const;
    bool isActive() const;
    void setActive(bool active);
};

#endif // ENEMY_H
