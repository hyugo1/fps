#ifndef WEAPON_H
#define WEAPON_H

#include <string>
#include <chrono>

class Weapon {
private:
    std::string name;
    float damage;
    int maxAmmo;
    int currentAmmo;
    float fireRate; // Shots per second
    std::chrono::steady_clock::time_point lastShotTime;

public:
    Weapon();
    Weapon(const std::string& name, float damage, int maxAmmo, float fireRate);

    bool canShoot() const;
    void shoot();
    void reload();

    std::string getName() const;
    float getDamage() const;
    int getCurrentAmmo() const;
    int getMaxAmmo() const;
    float getFireRate() const;
};

#endif // WEAPON_H
