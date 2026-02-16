#include <SDL2/SDL.h>
#include <stdio.h>

class Game {
    public:
        bool Init();
        bool IsRunning();
        void HandleEvents();
        void Update(float deltaTime);
        void Render();
        void Clean();
        Game();

    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        bool running;
        Uint32 lastTime;
        float playerX;
        float playerY;

        // Map
        static const int mapWidth = 16;
        static const int mapHeight = 16;
        int map[mapWidth * mapHeight];

        void DrawMap();
    };

Game::Game() {
    running = false;
    lastTime = 0;
    window = nullptr;
    renderer = nullptr;

    //player position
    playerX = 400;
    playerY = 300;
    
    int tempMap[mapWidth * mapHeight] = {
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
    };
    for(int i = 0; i < (mapWidth * mapHeight); i++)
        map[i] = tempMap[i];
}

void Game::DrawMap() {
    int tileSize = 50;

    float cameraX = playerX - 400;
    float cameraY = playerY - 300;

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int tile = map[y * mapWidth + x];

            if (tile == 0) continue;
            
            SDL_Rect rect = {
                (int)(x * tileSize - cameraX),
                (int)(y * tileSize - cameraY),
                tileSize,
                tileSize
            };

            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
};


bool Game::Init() {
    //initialise SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 0;
    }
    //create window
    window = SDL_CreateWindow(
        "FPS Game, using SDL2",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
            800, 600,
            SDL_WINDOW_SHOWN
        );
    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }
    //create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    running = true;
    return 1;
};
    
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

    playerX += dx * speed * deltaTime;
    playerY += dy * speed * deltaTime;
}

void Game::Render() {
    //clear screen to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //draw map
    DrawMap();
    //draw player
    SDL_Rect playerRect = { (int)playerX, (int)playerY, 50, 50 };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &playerRect);

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


int main(int argc, char* argv[]) {
    Game game;
    Uint32 lastTime = SDL_GetTicks();
    if (game.Init()) {
        while (game.IsRunning()) {
            Uint32 currentTime = SDL_GetTicks();
            float deltaTime = (currentTime - lastTime) / 1000.0f;
            lastTime = currentTime;
            game.HandleEvents();
            game.Update(deltaTime);
            game.Render();
        }
    }
    game.Clean();
    return 0;
}