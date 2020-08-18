cmake_minimum_required(VERSION 3.16)

project(lvrp VERSION 0.1 LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH ./${CMAKE_BUILD_TYPE})

find_package(SDL2 REQUIRED)
find_package(glLoadGen REQUIRED)
find_package(STB REQUIRED)
find_package(libmpv REQUIRED)
find_package(glm REQUIRED)

add_executable(lvrp
    ${GL_LOAD_GEN_DIR}/gl/gl_core_3_2.cpp
    src/main.cpp
    src/utils.hpp
    src/utils.cpp
    src/shaders.hpp
    src/shaders.cpp
    src/appwindow.hpp
    src/appwindow.cpp
    src/graphics.hpp
    src/graphics.cpp
    src/textures.hpp
    src/textures.cpp
    src/player.hpp
    src/player.cpp
    src/camera.hpp
    src/camera.cpp
)

target_include_directories(lvrp
    PUBLIC
        ${GL_LOAD_GEN_DIR}
        ${SDL2_INCLUDE_DIR_64}
        ${MPV_INCLUDE_DIR}
        ${STB_INCLUDE_DIR}
        ${GLM_INCLUDE_DIR}
)

target_link_directories(lvrp
    PUBLIC
        ${SDL2_LIBRARY_64}
        ${MPV_LIBRARY}
)

target_link_libraries(lvrp
    SDL2
    SDL2main
    opengl32
    mpv
)
