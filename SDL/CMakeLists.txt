cmake_minimum_required(VERSION 3.25)
project(fps)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
endif()


find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS})

add_executable(fps SDL2.cpp Game.cpp Weapon.cpp Enemy.cpp CombatSystem.cpp SpawnSystem.cpp)

target_link_libraries(fps ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES})


# @id:C_Cpp.default.includePath 

enable_testing()

add_executable(weapon_tests
    tests/weapon_tests.cpp
    Weapon.cpp
)

target_include_directories(weapon_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(weapon_tests PRIVATE ${SDL2_LIBRARIES})

add_test(NAME weapon_tests COMMAND weapon_tests)

add_executable(enemy_tests
    tests/enemies_tests.cpp
    Enemy.cpp
    Weapon.cpp
    CombatSystem.cpp
)

target_include_directories(enemy_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(enemy_tests PRIVATE ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

add_test(NAME enemy_tests COMMAND enemy_tests)


add_executable(player_tests
    tests/player_tests.cpp
    Enemy.cpp
    Weapon.cpp
    CombatSystem.cpp
)

target_include_directories(player_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(player_tests PRIVATE ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

add_test(NAME player_tests COMMAND player_tests)


add_executable(menu_tests
    tests/menu_tests.cpp
)

target_include_directories(menu_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(menu_tests PRIVATE ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})

add_test(NAME menu_tests COMMAND menu_tests)