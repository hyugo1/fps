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
            magSize = 18;
            currentAmmo = magSize;
            reload_cooldown = 0;
            isReloading = false;
            reloadTimer = 0.0f;
            reloadDuration = 2.0f;
            break;
        case RIFLE:
            fireRate = 5.0f;
            bulletSpeed = 500;
            bulletDamage = 100;
            requiredLevel = 2;
            magSize = 22;
            currentAmmo = magSize;
            reload_cooldown = 0;
            isReloading = false;
            reloadTimer = 0.0f;
            reloadDuration = 1.5f;
            break;
        case SHOTGUN:
            fireRate = 1.0f;
            bulletSpeed = 300;
            bulletDamage = 150;
            requiredLevel = 3;
            magSize = 5;
            currentAmmo = magSize;
            reload_cooldown = 0;
            isReloading = false;
            reloadTimer = 0.0f;
            reloadDuration = 2.5f;
            break;
        case MACHINEGUN:
            fireRate = 10.0f;
            bulletSpeed = 450;
            bulletDamage = 80;
            requiredLevel = 4;
            magSize = 100;
            currentAmmo = magSize;
            reload_cooldown = 0;
            isReloading = false;
            reloadTimer = 0.0f;
            reloadDuration = 3.0f;
            break;
    }
}
    
Weapon::Weapon() {
    type = PISTOL;
    fireRate = 2.0f;
    bulletSpeed = 400;
    bulletDamage = 200;
    requiredLevel = 1;
    magSize = 18;
    currentAmmo = magSize;
    reload_cooldown = 0;
    isReloading = false;
    reloadTimer = 0.0f;
    reloadDuration = 2.0f;
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
    if (isReloading || currentAmmo <= 0 || cooldown > 0.0f) {
        return;
    }
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
    currentAmmo--;
    if (currentAmmo == 0) {
        StartReload();
    }
    cooldown = 1.0f / fireRate;
}

void Weapon::StartReload() {
    if (isReloading || currentAmmo >= magSize) {
        return;
    }
    isReloading = true;
    reloadTimer = reloadDuration;
}

void Weapon::UpdateReloadCooldown(float deltaTime) {
    if (reload_cooldown > 0.0f) {
        reload_cooldown -= deltaTime;
    }
    if (isReloading) {
        reloadTimer -= deltaTime;
        if (reloadTimer <= 0.0f) {
            currentAmmo = magSize;
            isReloading = false;
        }
    }
}

float Weapon::GetReloadCooldown() const {
    return reload_cooldown;
}

float Weapon::GetCooldown() const {
    return cooldown;
}

void Weapon::UpdateCooldown(float deltaTime) {
    if(cooldown > 0.0f)
        cooldown -= deltaTime;
}

int Weapon::GetCurrentAmmo() const {
    return currentAmmo;
}

int Weapon::GetMagSize() const {
    return magSize;
}

bool Weapon::IsReloading() const {
    return isReloading;
}