# FPS game (C++ / SDL2)

A top-down shooter built in C++ using SDL2, with multiple enemy types, weapon progression, pickups, levels, UI states, and sprite-based rendering.


![Gameplay](assets/gameplay.png)

## Features

- Top-down movement and shooting
- Multiple weapons (pistol, rifle, shotgun, machinegun)
- Enemy variety (horizontal, vertical, smart/chasing)
- Item pickups (health, speed boost, weapons)
- Inventory and weapon switching
- Level progression + game over flow
- Texture/sprite rendering for player, enemies, walls, floor, and items

## Tech Stack

- C++20
- SDL2
- SDL2_image
- SDL2_ttf
- CMake

## Project Structure

- `SDL/` — main game source code
- `SDL/sprites/` — game assets (textures/sprites)
- `SDL/build/` — local build output

## Build & Run (macOS)

### 1) Install dependencies

```bash
brew install cmake pkg-config sdl2 sdl2_image sdl2_ttf
```

### 2) Configure + build

```bash
cd SDL
cmake -S . -B build
cmake --build build -j
```

### 3) Run

```bash
cd build
./fps
```

## Controls

- `W A S D` — move
- `Mouse Left Click` — shoot
- `R` — reload
- `Space` — melee
- `E` — open/close inventory
- `1 / 2 / 3 / 4` — switch weapon (when unlocked)
- `Esc` — pause/unpause / back from some menus
- `Enter` — continue next level / restart from game over
- `G` — reset high score on game-over screen

## Roadmap Ideas

- Add sound effects and music
- Add enemy hit feedback and damage popups
- Add save/load for progression
- Package releases for GitHub/itch.io



// console version (for testing game logic without graphics)
# RUN (ONLY ON MAC)

g++ -std=c++11 main.cpp -lncurses -o main

# THEN
./main
