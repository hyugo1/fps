cmake_minimum_required(VERSION 3.10)
project(FPSGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/Player.cpp
    src/Camera.cpp
    src/Weapon.cpp
    src/Enemy.cpp
    src/Game.cpp
)

set(HEADERS
    include/Player.h
    include/Camera.h
    include/Weapon.h
    include/Enemy.h
    include/Game.h
    include/Vector3.h
)

add_executable(fps_game ${SOURCES} ${HEADERS})

# Enable warnings
if(MSVC)
    target_compile_options(fps_game PRIVATE /W4)
else()
    target_compile_options(fps_game PRIVATE -Wall -Wextra -pedantic)
endif()
