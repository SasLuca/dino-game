cmake_minimum_required(VERSION 3.0)
project(kawaii-dino-game C)

set(CMAKE_C_STANDARD 99)

# Setting parameters for raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # or games

add_subdirectory(libs/raylib)

add_executable(kawaii-dino-game src/main.c)
target_link_libraries(kawaii-dino-game PRIVATE raylib)