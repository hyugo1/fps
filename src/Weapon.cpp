#include "Weapon.h"

Weapon::Weapon()
    : name("Pistol"), damage(10.0f), maxAmmo(12), currentAmmo(12), fireRate(2.0f) {
    lastShotTime = std::chrono::steady_clock::now();
}

Weapon::Weapon(const std::string& name, float damage, int maxAmmo, float fireRate)
    : name(name), damage(damage), maxAmmo(maxAmmo), currentAmmo(maxAmmo), fireRate(fireRate) {
    lastShotTime = std::chrono::steady_clock::now();
}

bool Weapon::canShoot() const {
    if (currentAmmo <= 0) return false;

    auto now = std::chrono::steady_clock::now();
    auto timeSinceLastShot = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - lastShotTime).count() / 1000.0f;

    return timeSinceLastShot >= (1.0f / fireRate);
}

void Weapon::shoot() {
    if (canShoot()) {
        currentAmmo--;
        lastShotTime = std::chrono::steady_clock::now();
    }
}

void Weapon::reload() {
    currentAmmo = maxAmmo;
}

std::string Weapon::getName() const {
    return name;
}

float Weapon::getDamage() const {
    return damage;
}

int Weapon::getCurrentAmmo() const {
    return currentAmmo;
}

int Weapon::getMaxAmmo() const {
    return maxAmmo;
}

float Weapon::getFireRate() const {
    return fireRate;
}
