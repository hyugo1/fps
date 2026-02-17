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

// ---------------- Constructor ----------------
Game::Game() {
    running = false;
    lastTime = 0;
    window = nullptr;
    renderer = nullptr;
    currentState = MENU;
    currentLevel = 1;
    playerHP = 30;
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
    //map
    for(int y = 0; y < mapHeight; y++) {
        for(int x = 0; x < mapWidth; x++) {
            if(x == 0 || y == 0 || x == mapWidth-1 || y == mapHeight-1)
                map[y * mapWidth + x] = 1; // border wall
            else if(rand() % 10 == 0)
                map[y * mapWidth + x] = 2; // random wall
            else
                map[y * mapWidth + x] = 0; // floor
        }
    }
    //enemies
    srand(time(nullptr));
    SpawnEnemies(5);
}

void Game::SpawnEnemies(int count) {
    const float MIN_DISTANCE = 200.0f;
    for(int i = 0; i < count; i++) {
        float spawnX, spawnY;
        bool collidesWithWall;
        float distance;
        do {
            spawnX = rand() % (mapWidth * tileSize);
            spawnY = rand() % (mapHeight * tileSize);
            // Check wall collision
            Entity temp; // create a temporary entity for collision checking
            temp.x = spawnX;
            temp.y = spawnY;
            temp.width = PLAYER_SIZE;
            temp.height = PLAYER_SIZE;
            collidesWithWall = DetectCollision(temp, spawnX, spawnY);
            // Check distance from player
            float dx = spawnX - player.x;
            float dy = spawnY - player.y;
            distance = sqrt(dx*dx + dy*dy);
        } while(collidesWithWall || distance < MIN_DISTANCE); // ensure enemies don't spawn too close to the player or inside walls
        Enemy::EnemyType type =
            static_cast<Enemy::EnemyType>(rand() % 3);
        enemies.push_back(Enemy(spawnX, spawnY, type));
    }
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
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // wall = grey
        SDL_RenderFillRect(renderer, &rect);
    }
    if (tile == 0) {
        SDL_Rect rect = {x*tileSize - cameraX, y*tileSize - cameraY, tileSize, tileSize};
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // dark grey floor
        SDL_RenderFillRect(renderer, &rect);
    }
    if (tile == 2) {
        SDL_Rect rect = {x*tileSize - cameraX, y*tileSize - cameraY, tileSize, tileSize};
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green for special tile
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

// Axis-Aligned Bounding Box collision detection
bool AABB(const Entity& a, const Entity& b) {
    return (a.x < b.x + b.width &&
            a.x + a.width > b.x &&
            a.y < b.y + b.height &&
            a.y + a.height > b.y);
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
    if (enemies.empty()) {
        currentState = LEVEL_COMPLETE;
    }
    if (keystate[SDL_SCANCODE_SPACE]) {
        for (auto &e : enemies) {
            if (AABB(player, e.getBody())) {
                e.TakeDamage(25);
            }
        }
    }
    for (auto &e : enemies) {
        e.Update(deltaTime,
         [this](const Entity& ent, float x, float y) {
             return DetectCollision(ent, x, y);
         },
         player.x, player.y);
    }
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](Enemy &e) { return e.IsDead(); }),
        enemies.end()
    );
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
    float speed = 200.0f; //200pixels per second
    float nextX = player.x + dx * speed * deltaTime;
    float nextY = player.y + dy * speed * deltaTime;
    // X collision
    if(!DetectCollision(player, nextX, player.y))
        player.x = nextX;
    // Y collision
    if(!DetectCollision(player, player.x, nextY))
        player.y = nextY;

    for (auto &e : enemies) {
        Entity enemyBody = e.getBody();
        // Check collision with player and enemy
        if (AABB(player, enemyBody)) {
                if (playerInvulnTimer <= 0.0f) {
                    playerHP -= 10;
                    playerInvulnTimer = 1.0f;
                }
            }
        }
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
        UpdateEnemy(deltaTime);
        UpdateCamera(deltaTime, dx, dy);
        UpdateClamp();
        DetectMouseClick();
        UpdateBullets(deltaTime);
    }

void Game::UpdateBullets(float deltaTime) {
    // Update bullets
    for (auto &b : bullets) {
        b.x += b.dx * b.speed * deltaTime;
        b.y += b.dy * b.speed * deltaTime;
    }
    // Remove bullets that hit walls
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [&](Bullet &b) {
                Entity bulletEntity{b.x, b.y, 5, 5};
                return DetectCollision(bulletEntity, b.x, b.y);
            }),
        bullets.end()
    );
    // Bullet–Enemy collision
    for (auto &b : bullets) {
        Entity bulletEntity{b.x, b.y, 5, 5};

        for (auto &e : enemies) {
            if (AABB(bulletEntity, e.getBody())) {
                e.TakeDamage(20);
                b.toDelete = true;
            }
        }
    }
    // Remove bullets marked for deletion
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [this](const Bullet &b){ return b.toDelete || DetectCollision(Entity{b.x,b.y,5,5}, b.x,b.y); }),
        bullets.end()
    );
}

void Game::DetectMouseClick() {
    int mouseX, mouseY;
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

    if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        if (shootCooldown <= 0.0f) {

            // Convert mouse from screen space to world space
            float worldMouseX = mouseX + cameraX;
            float worldMouseY = mouseY + cameraY;

            float dirX = worldMouseX - player.x;
            float dirY = worldMouseY - player.y;

            float length = sqrt(dirX * dirX + dirY * dirY);

            if (length != 0) {
                dirX /= length;
                dirY /= length;
            }

            Bullet b;
            b.x = player.x + player.width / 2;
            b.y = player.y + player.height / 2;
            b.dx = dirX;
            b.dy = dirY;
            b.speed = 400.0f;

            bullets.push_back(b);

            shootCooldown = 0.3f; // 0.3 sec between shots
        }
    }
}

void Game::UpdateLevelComplete() {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_RETURN]) {
        enemies.clear();
        SpawnEnemies(5 + currentLevel); // Spawn more enemies for next level
        currentState = PLAYING;
    }
}

void Game::UpdateGameOver() {
    // Handle game over logic (e.g., show message, wait for input)
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_RETURN]) {
        // Reset game state
        player.x = screenWidth / 2;
        player.y = screenHeight / 2;
        enemies.clear();
        SpawnEnemies(5);
        currentState = PLAYING;
        playerHP = 30;
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

    menu->Render("Press ENTER to Start");

    SDL_RenderPresent(renderer);
}

void Game::RenderLevelComplete() {
    //clear screen to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    menu->Render("Level Complete! Press ENTER");

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
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &playerRect);

    //draw enemies
    for (auto &e : enemies)
        e.Render(cameraX, cameraY, renderer);

    if (playerInvulnTimer > 0.0f)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // red flash
    else
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);


    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

    for (auto &b : bullets) {
        SDL_Rect rect = {
            (int)(b.x - cameraX),
            (int)(b.y - cameraY),
            5,
            5
        };
        SDL_RenderFillRect(renderer, &rect);
    }
    //update screen, swaps the back buffer to the screen
    //present
    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
}

void Game::RenderGameOver() {
    //clear screen to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    menu->Render("Game Over! Press ENTER");

    SDL_RenderPresent(renderer);
}

void Game::Clean() {
    // SDL_DestroyTexture(texture);
    delete menu;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
};