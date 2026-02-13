#include "Enemy.h"
#include "Player.h"

Enemy::Enemy() : position(0, 0, 0), health(50.0f), maxHealth(50.0f),
                 speed(2.0f), damage(10.0f), active(true) {}

Enemy::Enemy(const Vector3& pos, float maxHealth, float damage)
    : position(pos), health(maxHealth), maxHealth(maxHealth),
      speed(2.0f), damage(damage), active(true) {}

void Enemy::moveTowards(const Vector3& target, float deltaTime) {
    if (!active || !isAlive()) return;

    Vector3 direction = target - position;
    float distance = direction.length();

    if (distance > 0.1f) {
        direction = direction.normalize();
        position = position + direction * speed * deltaTime;
    }
}

void Enemy::takeDamage(float damage) {
    health -= damage;
    if (health < 0) {
        health = 0;
        active = false;
    }
}

void Enemy::attack(Player& player) {
    if (!active || !isAlive()) return;
    player.takeDamage(damage);
}

Vector3 Enemy::getPosition() const {
    return position;
}

void Enemy::setPosition(const Vector3& pos) {
    position = pos;
}

float Enemy::getHealth() const {
    return health;
}

bool Enemy::isAlive() const {
    return health > 0;
}

bool Enemy::isActive() const {
    return active;
}

void Enemy::setActive(bool active) {
    this->active = active;
}
