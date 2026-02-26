#include "Weapon.h"

#include <iostream>
#include <vector>

namespace {
int failures = 0;

void Expect(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "[FAIL] " << message << std::endl;
        failures++;
    }
}

void TestPistolDefaults() {
    Weapon pistol(Weapon::PISTOL);

    Expect(pistol.GetType() == Weapon::PISTOL, "Pistol type should be PISTOL");
    Expect(pistol.GetRequiredLevel() == 1, "Pistol required level should be 1");
    Expect(pistol.GetMagSize() == 18, "Pistol mag size should be 18");
    Expect(pistol.GetCurrentAmmo() == 18, "Pistol current ammo should start at 18");
    Expect(!pistol.IsReloading(), "Pistol should not start reloading");
}

void TestPistolFireCooldownAndAmmo() {
    Weapon pistol(Weapon::PISTOL);
    std::vector<Bullet> bullets;

    pistol.Fire(0.0f, 0.0f, 100.0f, 0.0f, bullets);

    Expect(bullets.size() == 1, "Pistol should spawn 1 bullet");
    Expect(pistol.GetCurrentAmmo() == 17, "Pistol should consume 1 ammo per shot");
    Expect(pistol.GetCooldown() > 0.0f, "Pistol fire should set cooldown");
}

void TestMachinegunFireRate() {
    Weapon machinegun(Weapon::MACHINEGUN);
    std::vector<Bullet> bullets;

    // First shot
    machinegun.Fire(0.0f, 0.0f, 100.0f, 0.0f, bullets);
    Expect(bullets.size() == 1, "Machinegun should spawn 1 bullet per shot");
    Expect(machinegun.GetCurrentAmmo() == 99, "Machinegun should consume 1 ammo per shot");
    Expect(machinegun.GetCooldown() > 0.0f, "Machinegun fire should set cooldown");

    // Should be able to fire again after enough cooldown time
    machinegun.UpdateCooldown(0.1f);
    machinegun.Fire(0.0f, 0.0f, 100.0f, 0.0f, bullets);
    Expect(bullets.size() == 2, "Machinegun should be able to fire again after cooldown");
    Expect(machinegun.GetCurrentAmmo() == 98, "Machinegun should consume ammo on second shot");

    // Empty magazine to trigger reload
    while (machinegun.GetCurrentAmmo() > 0) {
        machinegun.UpdateCooldown(1.0f);
        machinegun.Fire(0.0f, 0.0f, 100.0f, 0.0f, bullets);
    }

    Expect(machinegun.IsReloading(), "Machinegun should start reloading at 0 ammo");

    machinegun.UpdateReloadCooldown(3.1f);
    Expect(!machinegun.IsReloading(), "Machinegun should finish reloading after reload duration");
    Expect(machinegun.GetCurrentAmmo() == machinegun.GetMagSize(), "Reload should refill machinegun magazine");
}



void TestShotgunSpread() {
    Weapon shotgun(Weapon::SHOTGUN);
    std::vector<Bullet> bullets;

    shotgun.Fire(0.0f, 0.0f, 100.0f, 0.0f, bullets);

    Expect(bullets.size() == 5, "Shotgun should spawn 5 bullets");
    Expect(shotgun.GetCurrentAmmo() == 4, "Shotgun should consume 1 ammo per shot");
}

void TestReloadFlow() {
    Weapon pistol(Weapon::PISTOL);
    std::vector<Bullet> bullets;

    for (int i = 0; i < 18; ++i) {
        pistol.UpdateCooldown(1.0f);
        pistol.Fire(0.0f, 0.0f, 100.0f, 0.0f, bullets);
    }

    Expect(pistol.GetCurrentAmmo() == 0, "Ammo should be 0 after full magazine fired");
    Expect(pistol.IsReloading(), "Weapon should enter reload state when empty");

    pistol.UpdateReloadCooldown(2.1f);

    Expect(!pistol.IsReloading(), "Reload should complete after reload duration");
    Expect(pistol.GetCurrentAmmo() == pistol.GetMagSize(), "Reload should refill magazine");
}

void TestWeaponLevelMapping() {
    Weapon weapon;

    Expect(weapon.WeaponForLevel(Weapon::PISTOL) == 1, "PISTOL level should map to 1");
    Expect(weapon.WeaponForLevel(Weapon::RIFLE) == 2, "RIFLE level should map to 2");
    Expect(weapon.WeaponForLevel(Weapon::SHOTGUN) == 3, "SHOTGUN level should map to 3");
    Expect(weapon.WeaponForLevel(Weapon::MACHINEGUN) == 4, "MACHINEGUN level should map to 4");
}
}

int main() {
    TestPistolDefaults();
    TestPistolFireCooldownAndAmmo();
    TestMachinegunFireRate();
    TestShotgunSpread();
    TestReloadFlow();
    TestWeaponLevelMapping();

    if (failures == 0) {
        std::cout << "All weapon tests passed." << std::endl;
        return 0;
    }

    std::cerr << failures << " test(s) failed." << std::endl;
    return 1;
}
