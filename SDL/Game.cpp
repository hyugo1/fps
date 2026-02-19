//Game.cpp

#include <stdio.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include "Game.h"
#include "Enemy.h"
#include "Entity.h"
#include "Config.h"
#include "Menu.h"  
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Weapon.h"
#include "CombatSystem.h"
#include "SpawnSystem.h"

// ---------------- Constructor ----------------
Game::Game() {
    running = false;
    lastTime = 0;
    window = nullptr;
    renderer = nullptr;
    currentState = MENU;
    currentLevel = 1;
    playerHP = 30;
    playerMaxHP = 30;
    playerInvulnTimer = 0.0f;
    shootCooldown = 0.0f;
    screenHeight = 600;
    screenWidth = 800;
    cameraX = screenWidth / 2;
    cameraY = screenHeight / 2;
    player.x = screenWidth / 2;
    player.y = screenHeight / 2;
    player.width = PLAYER_SIZE;
    player.height = PLAYER_SIZE;
    tileSize = TILE_SIZE;
    playerMeleeDamage = 25;

    int playerTileX = (int)((player.x + player.width * 0.5f) / tileSize);
    int playerTileY = (int)((player.y + player.height * 0.5f) / tileSize);
    
    srand(time(nullptr)); // set rand before using it in map
    //map
    for(int y = 0; y < mapHeight; y++) {
        for(int x = 0; x < mapWidth; x++) {
            bool nearPlayerSpawn = std::abs(x - playerTileX) <= 1 && std::abs(y - playerTileY) <= 1;

            if(x == 0 || y == 0 || x == mapWidth-1 || y == mapHeight-1)
                map[y * mapWidth + x] = 1; // border wall
            else if(!nearPlayerSpawn && rand() % 10 == 0)
                map[y * mapWidth + x] = 2; // random wall
            else
                map[y * mapWidth + x] = 0; // floor
        }
    }
    //enemies
    SpawnSystem::SpawnEnemies(
        5,
        enemies,
        player,
        currentLevel,
        mapWidth,
        mapHeight,
        tileSize,
        [this](const Entity& ent, float x, float y) {
            return DetectCollision(ent, x, y);
        }
    );

    //weapons
    playerWeapons.push_back(Weapon(Weapon::PISTOL));
    currentWeaponIndex = 0;
}

int Game::getLevel() {
    return currentLevel;
}

void Game::DrawMap() {
    int startX = cameraX / tileSize;
    int startY = cameraY / tileSize;
    int endX = (cameraX + screenWidth) / tileSize + 1;
    int endY = (cameraY + screenHeight) / tileSize + 1;
    for(int y = startY; y < endY; y++)
        for(int x = startX; x < endX; x++)
            DrawTile(x, y);
};

void Game::DrawTile(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return;
    int tile = map[y * mapWidth + x];
    if (tile == 1) {
        SDL_Rect rect = {x*tileSize - cameraX, y*tileSize - cameraY, tileSize, tileSize};
        SDL_SetRenderDrawColor(renderer, 30, 30, 35, 255); //wall
        SDL_RenderFillRect(renderer, &rect);
    }
    if (tile == 0) {
        SDL_Rect rect = {x*tileSize - cameraX, y*tileSize - cameraY, tileSize, tileSize};
        SDL_SetRenderDrawColor(renderer, 60, 55, 50, 255); // floor
        SDL_RenderFillRect(renderer, &rect);
    }
    if (tile == 2) {
        SDL_Rect rect = {x*tileSize - cameraX, y*tileSize - cameraY, tileSize, tileSize};
        SDL_SetRenderDrawColor(renderer, 80, 90, 110, 255); //random obstacles
        SDL_RenderFillRect(renderer, &rect);
    }
}

// return true if collision, false if no collision
bool Game::DetectCollision(const Entity& entity, float nextX, float nextY) {
    int x = nextX;
    int y = nextY;
    int leftTile   = (int)(x / tileSize);
    int rightTile  = (int)((x + entity.width - 1) / tileSize);
    int topTile    = (int)(y / tileSize);
    int bottomTile = (int)((y + entity.height - 1) / tileSize);

    // Check each corner
    for (int tileY = topTile; tileY <= bottomTile; tileY++) {
        for (int tileX = leftTile; tileX <= rightTile; tileX++) {
            if (tileX < 0 || tileX >= mapWidth ||
                tileY < 0 || tileY >= mapHeight)
                return true;
            int tile = map[tileY * mapWidth + tileX];
            if (tile != 0)
                return true;
        }
    }
    return false;
}

bool Game::Init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Game Development",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenWidth, screenHeight,
        SDL_WINDOW_SHOWN);

    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    running = true;
    menu = new Menu(renderer);
    return true;
}

bool Game::IsRunning() {
    return running;
}

void Game::HandleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
    }
}



void Game::Update(float deltaTime) {
    if(currentState == MENU) {
        UpdateMenu();
    }
    else if(currentState == PLAYING) {
        float dx = 0.0f;
        float dy = 0.0f;
        HandlePlayerInput(deltaTime, dx, dy);
        HandleReloadInput();
        UpdateGame(deltaTime, dx, dy);
    }
    else if(currentState == LEVEL_COMPLETE) {
        UpdateLevelComplete();
    }
    else if(currentState == GAME_OVER) {
        UpdateGameOver();
    }
}

void Game::UpdateMenu() {
    // Handle menu logic (e.g., start game on key press)
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_RETURN]) {
        currentState = PLAYING;
    }
}

void Game::HandlePlayerInput(float deltaTime, float& dx, float& dy) {
   HandlePlayerMovementInput(deltaTime, dx, dy);
}

void Game::HandlePlayerMovementInput(float deltaTime, float& dx, float& dy) {
     const Uint8* keystate = SDL_GetKeyboardState(NULL);
    //direction vector
    dx = 0.0f;
    dy = 0.0f;
    if (keystate[SDL_SCANCODE_W])
        dy -= 1;
    if (keystate[SDL_SCANCODE_S])
        dy += 1;
    if (keystate[SDL_SCANCODE_A])
        dx -= 1;
    if (keystate[SDL_SCANCODE_D])
        dx += 1;
}

void Game::HandleInventoryInput() {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    static bool ePressedLastFrame = false;
    // Toggle inventory
    if (keystate[SDL_SCANCODE_E]) {
        if (!ePressedLastFrame) {
            inventoryOpen = !inventoryOpen;
        }
        ePressedLastFrame = true;
    } else {
        ePressedLastFrame = false;
    }

    if (inventoryOpen) {
        for (int i = 0; i < playerWeapons.size(); i++) {
        if (keystate[SDL_SCANCODE_1 + i]) {
            if (currentLevel >= playerWeapons[i].GetRequiredLevel()) {
                currentWeaponIndex = i;
                }
            }
        }
    }
}

void Game::HandleReloadInput() {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    static bool rPressedLastFrame = false;
    if (keystate[SDL_SCANCODE_R]) {
        if (!rPressedLastFrame) {
            if (!playerWeapons.empty()) {
                playerWeapons[currentWeaponIndex].StartReload();
            }
        }
        rPressedLastFrame = true;
    } else {
        rPressedLastFrame = false;
    }
}

void Game::UpdatePlayer(float deltaTime) {
    if (playerInvulnTimer > 0.0f)
        playerInvulnTimer -= deltaTime;

    if (playerHP <= 0) {
        currentState = GAME_OVER;
    }
    if (shootCooldown > 0.0f)
        shootCooldown -= deltaTime;
}

void Game::UpdateEnemy(float deltaTime) {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    bool levelComplete = false;
    CombatSystem::UpdateEnemy(
        deltaTime,
        enemies,
        player,
        playerMeleeDamage,
        [this](const Entity& ent, float x, float y) {
            return DetectCollision(ent, x, y);
        },
        keystate[SDL_SCANCODE_SPACE],
        levelComplete
    );

    if (levelComplete) {
        currentState = LEVEL_COMPLETE;
    }
}

void Game::UpdateCamera(float deltaTime, float dx, float dy) {
    cameraX = player.x - screenWidth / 2;
    cameraY = player.y - screenHeight / 2;

    int maxCameraX = mapWidth * tileSize - screenWidth;
    int maxCameraY = mapHeight * tileSize - screenHeight;
    if(cameraX > maxCameraX) cameraX = maxCameraX;
    if(cameraX < 0) cameraX = 0;
    if(cameraY > maxCameraY) cameraY = maxCameraY;
    if(cameraY < 0) cameraY = 0;
}

void Game::UpdateCollision(float deltaTime, float dx, float dy) {
    CombatSystem::UpdatePlayerCollision(
        deltaTime,
        dx,
        dy,
        player,
        enemies,
        playerInvulnTimer,
        playerHP,
        [this](const Entity& ent, float x, float y) {
            return DetectCollision(ent, x, y);
        }
    );
}

void Game::UpdateClamp() {
    // top left corner is the coords for the camera
    // Clamp keeps the view inside the world.
    cameraX = Clamp(cameraX, 0, mapWidth * tileSize - screenWidth);
    cameraY = Clamp(cameraY, 0, mapHeight * tileSize - screenHeight);
}

void Game::UpdateGame(float deltaTime, float dx, float dy) {
        UpdatePlayer(deltaTime);
        UpdateCollision(deltaTime, dx, dy);
        UpdateHealthItems(); 
        UpdateWeaponItems();
        UpdateEnemy(deltaTime);
        UpdateCamera(deltaTime, dx, dy);
        UpdateClamp();
        DetectMouseClick();
        UpdateWeaponCooldown(deltaTime);
        UpdateReloadCooldown(deltaTime);
        UpdateBullets(deltaTime);
        HandleInventoryInput();

}

void Game::UpdateWeaponCooldown(float deltaTime) {
    if (!playerWeapons.empty())
        playerWeapons[currentWeaponIndex].UpdateCooldown(deltaTime);
}

void Game::UpdateReloadCooldown(float deltaTime) {
    if (!playerWeapons.empty())
        playerWeapons[currentWeaponIndex].UpdateReloadCooldown(deltaTime);
}

void Game::UpdateBullets(float deltaTime) {
    CombatSystem::UpdateBullets(
        deltaTime,
        bullets,
        enemies,
        [this](const Entity& ent, float x, float y) {
            return DetectCollision(ent, x, y);
        }
    );
}

void Game::UpdateHealthItems() {
    for (auto &h : healthItems) {
        if (!h.collected) {
            Entity itemEntity{h.x, h.y, h.width, h.height};
            if (CombatSystem::AABB(player, itemEntity)) {
                playerHP += (int)(playerMaxHP * 0.2f); // heal 20%
                if (playerHP > playerMaxHP)
                    playerHP = playerMaxHP;
                h.collected = true;
            }
        }
    }

    healthItems.erase(
        std::remove_if(healthItems.begin(), healthItems.end(),
            [](const HealthItem &h){ return h.collected; }),
        healthItems.end()
    );
}

void Game::UpdateWeaponItems() {
    //only add weapon to inventory if not already owned
    if (currentLevel == 2) {
        // only spawn rifle in level 2
        for (auto &w : weaponItems) {
            if (w.type != Weapon::RIFLE) {
                w.collected = true;
            }
        }
    }

    if (currentLevel == 3) {
        // only spawn shotgun in level 3
        for (auto &w : weaponItems) {
            if (w.type != Weapon::SHOTGUN) {
                w.collected = true;
            }
        }
    }

    if (currentLevel == 4) {
        // only spawn machinegun in level 4
        for (auto &w : weaponItems) {
            if (w.type != Weapon::MACHINEGUN) {
                w.collected = true;
            }
        }
    }
    

    for (auto &w : weaponItems) {
        if (!w.collected) {
            Entity itemEntity{w.x, w.y, w.width, w.height};
            if (CombatSystem::AABB(player, itemEntity)) {
                // Add weapon to inventory if not already owned
                bool alreadyOwned = false;
                for (auto &wp : playerWeapons) {
                    if (wp.GetType() == w.type) {
                        alreadyOwned = true;
                        break;
                    }
                }
                if (!alreadyOwned)
                    playerWeapons.push_back(Weapon(w.type));

                w.collected = true;
            }
        }
    }

    // remove collected items
    weaponItems.erase(
        std::remove_if(weaponItems.begin(), weaponItems.end(),
            [](const WeaponItem &w){ return w.collected; }),
        weaponItems.end()
    );
}

void Game::DetectMouseClick() {
    CombatSystem::DetectMouseClick(
        cameraX,
        cameraY,
        player,
        playerWeapons,
        currentWeaponIndex,
        bullets
    );
}

void Game::UpdateLevelComplete() {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_RETURN]) {
        currentLevel++;
        enemies.clear();
        bullets.clear();
        weaponItems.clear();
        SpawnSystem::SpawnEnemies(
            5 + currentLevel,
            enemies,
            player,
            currentLevel,
            mapWidth,
            mapHeight,
            tileSize,
            [this](const Entity& ent, float x, float y) {
                return DetectCollision(ent, x, y);
            }
        );
        SpawnSystem::SpawnHealthItems(
            2,
            healthItems,
            player,
            mapWidth,
            mapHeight,
            tileSize,
            [this](const Entity& ent, float x, float y) {
                return DetectCollision(ent, x, y);
            }
        );
        SpawnSystem::SpawnWeaponItems(
            1,
            weaponItems,
            player,
            currentLevel,
            mapWidth,
            mapHeight,
            tileSize,
            [this](const Entity& ent, float x, float y) {
                return DetectCollision(ent, x, y);
            }
        );
        currentState = PLAYING;
    }
}

void Game::UpdateGameOver() {
    // Handle game over logic (e.g., show message, wait for input)
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_RETURN]) {
        // Reset game state
        currentLevel = 1;
        player.x = screenWidth / 2;
        player.y = screenHeight / 2;
        enemies.clear();
        bullets.clear();
        healthItems.clear();
        weaponItems.clear();
        playerWeapons.clear();
        playerWeapons.push_back(Weapon(Weapon::PISTOL));
        currentWeaponIndex = 0;
        SpawnSystem::SpawnEnemies(
            5,
            enemies,
            player,
            currentLevel,
            mapWidth,
            mapHeight,
            tileSize,
            [this](const Entity& ent, float x, float y) {
                return DetectCollision(ent, x, y);
            }
        );
        SpawnSystem::SpawnHealthItems(
            2,
            healthItems,
            player,
            mapWidth,
            mapHeight,
            tileSize,
            [this](const Entity& ent, float x, float y) {
                return DetectCollision(ent, x, y);
            }
        );
        SpawnSystem::SpawnWeaponItems(
            1,
            weaponItems,
            player,
            currentLevel,
            mapWidth,
            mapHeight,
            tileSize,
            [this](const Entity& ent, float x, float y) {
                return DetectCollision(ent, x, y);
            }
        );
        currentState = PLAYING;
        playerHP = 30;
        playerMaxHP = 30;
        playerInvulnTimer = 0.0f;
    }
}

double Game::Clamp(double a, double minimum, double maximum) {
    if (a < minimum) return minimum;
    if (a > maximum) return maximum;
    return a;
}

void Game::Render() {
    if(currentState == MENU) {
        RenderMenu();
    }
    else if(currentState == PLAYING) {
        RenderGame();
    }
    else if(currentState == LEVEL_COMPLETE) {
        RenderLevelComplete();
    }
    else if(currentState == GAME_OVER) {
        RenderGameOver();
    }
}   

void Game::RenderMenu() {
    //clear screen to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    menu->Render("Press ENTER to Start", screenWidth, screenHeight);

    SDL_RenderPresent(renderer);
}

void Game::RenderLevelComplete() {
    //clear screen to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    menu->Render("Level Complete! Press ENTER", screenWidth, screenHeight);

    SDL_RenderPresent(renderer);
}

void Game::RenderGame() {
    //clear screen to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //draw map
    DrawMap();
    //draw player
    SDL_Rect playerRect = { 
        (int)(player.x - cameraX), 
        (int)(player.y - cameraY), 
        (int)player.width, (int)player.height
    };
    if (playerInvulnTimer > 0.0f)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // red flash
    else
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        
    SDL_RenderFillRect(renderer, &playerRect);
    PlayerHP();
    DisplayAmmo();
    
    //draw enemies
    for (auto &e : enemies)
        e.Render(cameraX, cameraY, renderer);
    EnemyHP();
    


    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

    for (auto &b : bullets) {
        SDL_Rect rect = {
            (int)(b.x - cameraX),
            (int)(b.y - cameraY),
            5, 5
        };
        SDL_RenderFillRect(renderer, &rect);
    }

    for (auto &h : healthItems) {
        if (!h.collected) {
            SDL_Rect rect = {
                (int)(h.x - cameraX),
                (int)(h.y - cameraY),
                (int)h.width,
                (int)h.height
            };
            SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    for (auto &w : weaponItems) {
        if (!w.collected) {
            SDL_Rect rect = {
                (int)(w.x - cameraX),
                (int)(w.y - cameraY),
                (int)w.width,
                (int)w.height
            };
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // blue for weapons
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    if (inventoryOpen) {
        int startX = 50;
        int startY = 50;
        int size = 40;
        for (size_t i = 0; i < playerWeapons.size(); i++) {
            SDL_Rect rect = { startX + (int)i*(size+10), startY, size, size };
            if (i == currentWeaponIndex)
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // highlight current
            else
                SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    //update screen, swaps the back buffer to the screen
    //present
    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
}

void Game::PlayerHP() {
    float hpRatio = (float)playerHP / (float)playerMaxHP;
    SDL_Rect hpBarBack = { (int)(player.x - cameraX), (int)(player.y - cameraY - 10), (int)player.width, 5 };
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // dark gray background
    SDL_RenderFillRect(renderer, &hpBarBack);

    SDL_Rect hpBarFront = { (int)(player.x - cameraX), (int)(player.y - cameraY - 10), (int)(player.width * hpRatio), 5 };
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
    SDL_RenderFillRect(renderer, &hpBarFront);
}


void Game::EnemyHP() {
    for (auto &e : enemies) {
        float hpRatio = (float)e.GetHP() / (float)e.GetMaxHP();
        Entity enemyBody = e.getBody();
        SDL_Rect hpBarBack = { (int)(enemyBody.x - cameraX), (int)(enemyBody.y - cameraY - 10), (int)enemyBody.width, 5 };
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); 
        SDL_RenderFillRect(renderer, &hpBarBack);

        SDL_Rect hpBarFront = { (int)(enemyBody.x - cameraX), (int)(enemyBody.y - cameraY - 10), (int)(enemyBody.width * hpRatio), 5 };
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
        SDL_RenderFillRect(renderer, &hpBarFront);
    }
}

void Game::DisplayAmmo() {
    if (playerWeapons.empty()) return;
    
    Weapon& currentWeapon = playerWeapons[currentWeaponIndex];
    int currentAmmo = currentWeapon.GetCurrentAmmo();
    int magSize = currentWeapon.GetMagSize();
    bool reloading = currentWeapon.IsReloading();
    
    // Position in bottom-right corner
    int barWidth = 150;
    int barHeight = 30;
    int padding = 10;
    int x = screenWidth - barWidth - padding;
    int y = screenHeight - barHeight - padding;
    
    // Background
    SDL_Rect bgRect = { x, y, barWidth, barHeight };
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 200);
    SDL_RenderFillRect(renderer, &bgRect);
    
    // Ammo bar
    float ammoRatio = (float)currentAmmo / (float)magSize;
    SDL_Rect ammoBar = { x + 5, y + 5, (int)((barWidth - 10) * ammoRatio), barHeight - 10 };
    
    if (reloading) {
        SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // Orange when reloading
    } else if (currentAmmo == 0) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red when empty
    } else if (currentAmmo <= magSize * 0.25f) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow when low
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green when good
    }
    SDL_RenderFillRect(renderer, &ammoBar);
    
    // Border
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &bgRect);

    static TTF_Font* ammoFont = TTF_OpenFont("BitcountGridDouble.ttf", 18);
    if (ammoFont) {
        char ammoText[32];
        std::snprintf(ammoText, sizeof(ammoText), "%d/%d", currentAmmo, magSize);

        SDL_Color textColor = {255, 255, 255, 255};
        SDL_Surface* ammoTextSurface = TTF_RenderText_Solid(ammoFont, ammoText, textColor);
        if (ammoTextSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, ammoTextSurface);
            if (textTexture) {
                SDL_Rect textRect;
                textRect.w = ammoTextSurface->w;
                textRect.h = ammoTextSurface->h;
                textRect.x = x + (barWidth - textRect.w) / 2;
                textRect.y = y + (barHeight - textRect.h) / 2;
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(ammoTextSurface);
        }

        if (reloading) {
            static TTF_Font* reloadFont = TTF_OpenFont("BitcountGridDouble.ttf", 14);
            if (reloadFont) {
                SDL_Color reloadColor = {255, 165, 0, 255};
                SDL_Surface* reloadSurface = TTF_RenderText_Solid(reloadFont, "RELOADING", reloadColor);
                if (reloadSurface) {
                    SDL_Texture* reloadTexture = SDL_CreateTextureFromSurface(renderer, reloadSurface);
                    if (reloadTexture) {
                        SDL_Rect reloadRect;
                        reloadRect.w = reloadSurface->w;
                        reloadRect.h = reloadSurface->h;
                        reloadRect.x = x + (barWidth - reloadRect.w) / 2;
                        reloadRect.y = y - reloadRect.h - 4;
                        SDL_RenderCopy(renderer, reloadTexture, nullptr, &reloadRect);
                        SDL_DestroyTexture(reloadTexture);
                    }
                    SDL_FreeSurface(reloadSurface);
                }
            }
        }
    }
}

void Game::RenderGameOver() {
    //clear screen to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    menu->Render("Game Over! Press ENTER", screenWidth, screenHeight);

    SDL_RenderPresent(renderer);
}

void Game::Clean() {
    // SDL_DestroyTexture(texture);
    delete menu;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
};