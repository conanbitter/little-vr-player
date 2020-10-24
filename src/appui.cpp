#include "appui.hpp"
#include <iostream>
#include <sstream>

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

void timeToString(double timeStamp, std::stringstream& stream) {
    int intTime = (int)timeStamp;
    int hh = intTime / 3600;
    int mm = (intTime % 3600) / 60;
    int ss = (intTime % 3600) % 60;
    stream << hh << ":";
    if (mm < 10) stream << "0";
    stream << mm << ":";
    if (ss < 10) stream << "0";
    stream << ss;
}

AppUI::AppUI(SDL_Window* window) : duration{0.0}, position{0.0}, paused{true}, _window{window} {
    int i;
};

void AppUI::onChangeTitle(string newTitle) {
    title = newTitle;
    updateTitle();
}

void AppUI::onChangeDuration(double newDuration) {
    duration = newDuration;
    updateTitle();
}
void AppUI::onChangePos(double newPos) {
    position = newPos;
    updateTitle();
}
void AppUI::onChangePause(bool isPaused) {}
void AppUI::onResize(int newWidth, int newHeight) {}

void AppUI::updateTitle() {
    std::stringstream result;
    result << "[ ";
    timeToString(position, result);
    result << " / ";
    timeToString(duration, result);
    result << " ] " << title;
    SDL_SetWindowTitle(_window, result.str().c_str());
}