# FPS Game (C++)

A basic First-Person Shooter game implementation in C++ demonstrating core FPS game mechanics.

## Features

- **Player System**: First-person camera control with movement and rotation
- **Weapon System**: Shooting mechanics with ammo management and fire rate
- **Enemy AI**: Basic enemy behavior with movement towards player and attacking
- **Game Loop**: Core game loop with delta time, wave system, and scoring
- **Object-Oriented Design**: Clean class structure with proper encapsulation

## Project Structure

```
fps/
├── include/          # Header files
│   ├── Camera.h     # FPS camera system
│   ├── Player.h     # Player class with health and movement
│   ├── Weapon.h     # Weapon system with shooting mechanics
│   ├── Enemy.h      # Enemy AI and behavior
│   ├── Game.h       # Main game class and game loop
│   └── Vector3.h    # 3D vector math utilities
├── src/             # Implementation files
│   ├── Camera.cpp
│   ├── Player.cpp
│   ├── Weapon.cpp
│   ├── Enemy.cpp
│   ├── Game.cpp
│   └── main.cpp     # Entry point
├── assets/          # Game assets (textures, models, etc.)
└── CMakeLists.txt   # Build configuration
```

## Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.10 or higher

## Building

### Linux/macOS

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .

# Run the game
./fps_game
```

### Windows

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build . --config Release

# Run the game
Release\fps_game.exe
```

## Game Mechanics

### Player
- Health system (default: 100 HP)
- Movement system with configurable speed
- First-person camera with yaw and pitch rotation
- Weapon management

### Weapons
- Damage values
- Ammo system with magazine capacity
- Fire rate limiting
- Reload functionality

### Enemies
- Health system
- AI that moves towards player
- Attack capability when in range
- Wave-based spawning

### Game System
- Delta time-based updates
- Wave system with increasing difficulty
- Score tracking
- Game over when player dies

## Implementation Details

This is a console-based simulation that demonstrates the core architecture of an FPS game:

- **Vector3**: Basic 3D vector math for positions and directions
- **Camera**: FPS camera with forward/right/up vectors and rotation
- **Player**: Combines camera with health, movement, and weapon systems
- **Weapon**: Implements shooting mechanics with timing and ammo
- **Enemy**: Basic AI that tracks and attacks the player
- **Game**: Main game loop that manages all systems and game state

## Future Enhancements

To turn this into a fully-featured FPS game, you could add:

- 3D graphics rendering (OpenGL, DirectX, or Vulkan)
- Input handling for keyboard and mouse
- Collision detection with level geometry
- Multiple weapon types
- Power-ups and items
- Sound effects and music
- Multiplayer support
- Level design and map loading
- Particle effects
- HUD and UI system

## License

MIT License - Feel free to use this code for learning and development.