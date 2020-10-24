#include "appui.hpp"
#include <iostream>

const string vertexShaderCodeUI = R"(
    #version 410

    layout(location = 0) in vec2 vert;
    layout(location = 1) in vec2 vertUV;
    layout(location = 2) in vec4 color;

    out vec2 fragUV;
    out vec4 fragColor;

    uniform int screenWidth;
    uniform int screeHeight;

    void main() {
        gl_Position = vec4(vert.x/screenWidth, vert.y/screenHeight, 0.0, 1.0);
        fragUV = vertUV;
    }
)";

const string fragmentShaderCodeUI = R"(
    #version 410
    //uniform sampler2D tex;

    in vec2 fragUV;

    layout(location = 0) out vec4 outputColor;

    void main() {
        //outputColor = texture(tex, vec2(fragUV.x/2,fragUV.y));
    }
)";

AppUI::AppUI(SDL_Window* window) : duration{0.0}, position{0.0}, paused{true}, _window{window} {
    int i;
};

void AppUI::onChangeTitle(string newTitle) {
    string title = string("[LVRP] ") + newTitle;
    std::cout << newTitle << std::endl;
    SDL_SetWindowTitle(_window, title.c_str());
}

void AppUI::onChangeDuration(double newDuration) {}
void AppUI::onChangePos(double newPos) {}
void AppUI::onChangePause(bool isPaused) {}
void AppUI::onResize(int newWidth, int newHeight) {}