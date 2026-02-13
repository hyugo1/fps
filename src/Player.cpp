#include "Player.h"
#include "Weapon.h"

Player::Player() : camera(), health(100.0f), maxHealth(100.0f), speed(5.0f) {}

Player::Player(const Vector3& startPosition, float maxHealth)
    : camera(startPosition), health(maxHealth), maxHealth(maxHealth), speed(5.0f) {}

void Player::move(float forward, float right, float deltaTime) {
    float distance = speed * deltaTime;
    camera.moveForward(forward * distance);
    camera.moveRight(right * distance);
}

void Player::rotate(float deltaYaw, float deltaPitch) {
    camera.rotate(deltaYaw, deltaPitch);
}

void Player::takeDamage(float damage) {
    health -= damage;
    if (health < 0) health = 0;
}

void Player::heal(float amount) {
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

void Player::setWeapon(std::shared_ptr<Weapon> weapon) {
    currentWeapon = weapon;
}

void Player::shoot() {
    if (currentWeapon && currentWeapon->canShoot()) {
        currentWeapon->shoot();
    }
}

Camera& Player::getCamera() {
    return camera;
}

const Camera& Player::getCamera() const {
    return camera;
}

float Player::getHealth() const {
    return health;
}

float Player::getMaxHealth() const {
    return maxHealth;
}

bool Player::isAlive() const {
    return health > 0;
}

Vector3 Player::getPosition() const {
    return camera.getPosition();
}
