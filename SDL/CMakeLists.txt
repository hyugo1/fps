cmake_minimum_required(VERSION 3.25)
project(fps)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
endif()


# Find SDL2 and SDL2_image using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS})

add_executable(fps SDL2.cpp Game.cpp Enemy.cpp)
target_link_libraries(fps ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})