#include "Menu.h"
#include <iostream>

namespace {
int failures = 0;

void Expect(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "[FAIL] " << message << std::endl;
        failures++;
    }
}

void TestMainMenuActions() {
    int mouseX = 0;
    int mouseY = 0;
    bool leftDown = false;

    Menu menu(nullptr, [&](int* x, int* y) {
        *x = mouseX;
        *y = mouseY;
        return leftDown ? SDL_BUTTON(SDL_BUTTON_LEFT) : 0;
    });

    const int screenWidth = 800;
    const int screenHeight = 600;

    mouseX = 400;
    mouseY = 220;
    leftDown = true;
    Expect(menu.UpdateMainMenu(screenWidth, screenHeight) == Menu::START, "Clicking start should return START");

    leftDown = false;
    menu.UpdateMainMenu(screenWidth, screenHeight);

    mouseX = 400;
    mouseY = 300;
    leftDown = true;
    Expect(menu.UpdateMainMenu(screenWidth, screenHeight) == Menu::OPTIONS, "Clicking options should return OPTIONS");

    leftDown = false;
    menu.UpdateMainMenu(screenWidth, screenHeight);

    mouseX = 400;
    mouseY = 380;
    leftDown = true;
    Expect(menu.UpdateMainMenu(screenWidth, screenHeight) == Menu::EXIT, "Clicking exit should return EXIT");
}

void TestOptionsMenuActions() {
    int mouseX = 0;
    int mouseY = 0;
    bool leftDown = false;

    Menu menu(nullptr, [&](int* x, int* y) {
        *x = mouseX;
        *y = mouseY;
        return leftDown ? SDL_BUTTON(SDL_BUTTON_LEFT) : 0;
    });

    const int screenWidth = 800;
    const int screenHeight = 600;

    mouseX = 400;
    mouseY = 220;
    leftDown = true;
    Expect(menu.UpdateOptionsMenu(screenWidth, screenHeight) == Menu::EASY, "Clicking easy should return EASY");

    leftDown = false;
    menu.UpdateOptionsMenu(screenWidth, screenHeight);

    mouseX = 400;
    mouseY = 300;
    leftDown = true;
    Expect(menu.UpdateOptionsMenu(screenWidth, screenHeight) == Menu::MEDIUM, "Clicking medium should return MEDIUM");

    leftDown = false;
    menu.UpdateOptionsMenu(screenWidth, screenHeight);

    mouseX = 400;
    mouseY = 380;
    leftDown = true;
    Expect(menu.UpdateOptionsMenu(screenWidth, screenHeight) == Menu::HARD, "Clicking hard should return HARD");
}

void TestNoRepeatWhileHeld() {
    int mouseX = 400;
    int mouseY = 220;
    bool leftDown = true;

    Menu menu(nullptr, [&](int* x, int* y) {
        *x = mouseX;
        *y = mouseY;
        return leftDown ? SDL_BUTTON(SDL_BUTTON_LEFT) : 0;
    });

    const int screenWidth = 800;
    const int screenHeight = 600;

    Expect(menu.UpdateMainMenu(screenWidth, screenHeight) == Menu::START, "First press should trigger START");
    Expect(menu.UpdateMainMenu(screenWidth, screenHeight) == Menu::NONE, "Held mouse button should not trigger repeatedly");

    leftDown = false;
    menu.UpdateMainMenu(screenWidth, screenHeight);

    leftDown = true;
    Expect(menu.UpdateMainMenu(screenWidth, screenHeight) == Menu::START, "Pressing again after release should trigger START again");
}
}

int main() {
    TestMainMenuActions();
    TestOptionsMenuActions();
    TestNoRepeatWhileHeld();

    if (failures == 0) {
        std::cout << "All menu tests passed." << std::endl;
        return 0;
    }

    std::cerr << failures << " test(s) failed." << std::endl;
    return 1;
}
