cmake_minimum_required(VERSION 3.16)

project(lvrp VERSION 0.1 LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH ./${CMAKE_BUILD_TYPE})

add_executable(lvrp
    src/main.cpp
)

find_package(SDL2 REQUIRED)

target_include_directories(lvrp
    PUBLIC
        ${SDL2_INCLUDE_DIR_64}
)

target_link_directories(lvrp
    PUBLIC
        ${SDL2_LIBRARY_64}
)

target_link_libraries(lvrp
    SDL2
    SDL2main
)
