//Game.cpp

#include "Game.h"
#include <stdio.h>
#include <cmath>
#include <vector>
#include "Enemy.h"
#include "Entity.h"
#include "Config.h"

// ---------------- Constructor ----------------
Game::Game() {
    running = false;
    lastTime = 0;
    window = nullptr;
    renderer = nullptr;

    screenHeight = 600;
    screenWidth = 800;

    cameraX = screenWidth / 2;
    cameraY = screenHeight / 2;

    player.x = screenWidth / 2;
    player.y = screenHeight / 2;
    player.width = PLAYER_SIZE;
    player.height = PLAYER_SIZE;

    tileSize = TILE_SIZE;

    //enemies
    enemies.push_back(Enemy());

    //map
    int tempMap[mapWidth * mapHeight] = {
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
    };
    for(int i = 0; i < (mapWidth * mapHeight); i++)
        map[i] = tempMap[i];
}

void Game::DrawMap() {
    int startX = cameraX / tileSize;
    int startY = cameraY / tileSize;

    int endX = (cameraX + screenWidth) / tileSize + 1;
    int endY = (cameraY + screenHeight) / tileSize + 1;

    for(int y = startY; y < endY; y++)
        for(int x = startX; x < endX; x++)
            drawTile(x, y);
};

void Game::drawTile(int x, int y) {
    int tile = map[y * mapWidth + x];

    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return;
    
    if (tile == 1) {
        SDL_Rect rect = {
            (int)(x * tileSize - cameraX),
            (int)(y * tileSize - cameraY),
            tileSize,
            tileSize
        };

        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // wall = grey
        SDL_RenderFillRect(renderer, &rect);
    }

    if (tile == 0) {
        SDL_Rect rect = {x*tileSize - cameraX, y*tileSize - cameraY, tileSize, tileSize};
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // dark grey floor
        SDL_RenderFillRect(renderer, &rect);
    }
}


bool Game::detectCollision(const Entity& entity, float nextX, float nextY) {
    int x = nextX;
    int y = nextY;

    int leftTile   = (int)(x / tileSize);
    int rightTile  = (int)((x + entity.width - 1) / tileSize);
    int topTile    = (int)(y / tileSize);
    int bottomTile = (int)((y + entity.height - 1) / tileSize);

    // Check each corner
    if(map[topTile * mapWidth + leftTile] == 1) return false;
    if(map[topTile * mapWidth + rightTile] == 1) return false;
    if(map[bottomTile * mapWidth + leftTile] == 1) return false;
    if(map[bottomTile * mapWidth + rightTile] == 1) return false;

    return true;
}

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

//add player movement, physics, enemy updates, bullet updates
//deltaTime =  time since last frame
void Game::Update(float deltaTime) {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    int playerSize = 50;

    //direction vector
    float dx = 0.0f;
    float dy = 0.0f;

    // (0,0) is the top left corner
    // x goes right
    // y goes down
    if (keystate[SDL_SCANCODE_W])
        dy -= 1;
    if (keystate[SDL_SCANCODE_S])
        dy += 1;
    if (keystate[SDL_SCANCODE_A])
        dx -= 1;
    if (keystate[SDL_SCANCODE_D])
        dx += 1;


    float length = sqrt(dx * dx + dy * dy);

    if (length != 0.0f) {
        dx /= length;
        dy /= length;
    };

    //200pixels per second
    float speed = 200.0f;

    cameraX = player.x - screenWidth / 2;
    cameraY = player.y - screenHeight / 2;

    int maxCameraX = mapWidth * tileSize - screenWidth;
    int maxCameraY = mapHeight * tileSize - screenHeight;

    if(cameraX > maxCameraX) cameraX = maxCameraX;
    if(cameraY > maxCameraY) cameraY = maxCameraY;

    float nextX = player.x + dx * speed * deltaTime;
    float nextY = player.y + dy * speed * deltaTime;
    
    // X collision
    if(detectCollision(player, nextX, player.y))
        player.x = nextX;
    // else
    //     nextX -= 0.1f;
    
    // Y collision
    if(detectCollision(player, player.x, nextY))
        player.y = nextY;
    // else
    //     nextY -= 0.1f;

    for (auto &e : enemies) {
        Entity enemyBody = e.getBody();
        if (AABB(player, enemyBody)) {
            running = false;
        }
    }

    // top left corner is the coords for the camera

    // clamp keeps the view inside the world.
    cameraX = clamp(cameraX, 0, mapWidth * tileSize - screenWidth);
    cameraY = clamp(cameraY, 0, mapHeight * tileSize - screenHeight);

    for (auto &e : enemies)
        e.Update(deltaTime, map, mapWidth, mapHeight);

}

double Game::clamp(double a, double minimum, double maximum) {
    if (a < minimum) return minimum;
    if (a > maximum) return maximum;
    return a;
}

void Game::Render() {
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

    //update screen, swaps the back buffer to the screen
    //present
    SDL_RenderPresent(renderer);
}


void Game::Clean() {
    // SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
};