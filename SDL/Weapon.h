// Weapon.h
#ifndef WEAPON_H
#define WEAPON_H

#include <SDL2/SDL.h>
#include <vector>
#include "Entity.h"

struct Bullet {
    float x, y;
    float dx, dy;
    float speed;
    int damage;
    bool toDelete = false;
};

class Weapon {
public:
    enum WeaponType { PISTOL, SHOTGUN, RIFLE, MACHINEGUN };

    Weapon(WeaponType type = PISTOL);

    WeaponType GetType() const;

    void Fire(float startX, float startY, float targetX, float targetY, std::vector<Bullet>& bullets);
    float GetCooldown() const;
    void UpdateCooldown(float deltaTime);
    int GetRequiredLevel() const;
    int WeaponForLevel(WeaponType type);

    void UpdateReloadCooldown(float deltaTime);
    float GetReloadCooldown() const;
    void StartReload();
    int GetCurrentAmmo() const;
    int GetMagSize() const;
    bool IsReloading() const;

private:
    WeaponType type;
    float fireRate;
    float cooldown;
    int bulletSpeed;
    int bulletDamage;
    int requiredLevel;
    float reload_cooldown;
    int magSize;
    int currentAmmo;
    bool isReloading;
    float reloadTimer;
    float reloadDuration;
};

struct WeaponItem {
    float x, y;
    float width, height;
    Weapon::WeaponType type;
    bool collected = false;
};

#endif // WEAPON_H