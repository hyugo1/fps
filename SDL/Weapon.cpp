// Weapon.cpp

#include "Weapon.h"
#include <cmath>
#include <cstdlib>

Weapon::Weapon(WeaponType type) : type(type), cooldown(0.0f) {
    switch(type) {
        case PISTOL:
            fireRate = 2.0f;
            bulletSpeed = 400;
            bulletDamage = 200;
            requiredLevel = 1;
            break;
        case RIFLE:
            fireRate = 5.0f;
            bulletSpeed = 500;
            bulletDamage = 100;
            requiredLevel = 2;
            break;
        case SHOTGUN:
            fireRate = 1.0f;
            bulletSpeed = 300;
            bulletDamage = 150;
            requiredLevel = 3;
            break;
        case MACHINEGUN:
            fireRate = 10.0f;
            bulletSpeed = 450;
            bulletDamage = 80;
            requiredLevel = 4;
            break;
    }
}

Weapon::WeaponType Weapon::GetType() const {
    return type;
}

int Weapon::WeaponForLevel(WeaponType type) {
    switch(type) {
        case PISTOL: return 1;
        case RIFLE: return 2;
        case SHOTGUN: return 3;
        case MACHINEGUN: return 4;
    }
    return 1; // default
}


int Weapon::GetRequiredLevel() const {
    return requiredLevel;
}

void Weapon::Fire(float startX, float startY, float targetX, float targetY, std::vector<Bullet>& bullets) {
    if(cooldown > 0.0f) return; // still in cooldown

    float dx = targetX - startX;
    float dy = targetY - startY;
    float length = sqrt(dx*dx + dy*dy);
    if(length != 0) {
        dx /= length;
        dy /= length;
    }

    if(type == SHOTGUN) {
        // Fire 5 bullets with a spread
        for(int i = -2; i <= 2; i++) {
            float angle = atan2(dy, dx) + i * 0.1f; // small spread
            bullets.push_back({startX, startY, cos(angle), sin(angle), (float)bulletSpeed, bulletDamage});
        }
    } else {
        bullets.push_back({startX, startY, dx, dy, (float)bulletSpeed, bulletDamage});
    }
    cooldown = 1.0f / fireRate;
}

float Weapon::GetCooldown() const {
    return cooldown;
}

void Weapon::UpdateCooldown(float deltaTime) {
    if(cooldown > 0.0f)
        cooldown -= deltaTime;
};