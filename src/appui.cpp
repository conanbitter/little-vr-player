#include "appui.hpp"
#include <iostream>

void AppUI::onChangeTitle(string newTitle) {
    string title = string("[LVRP] ") + newTitle;
    std::cout << newTitle << std::endl;
    SDL_SetWindowTitle(_window, title.c_str());
}

void AppUI::onChangeDuration(double newDuration) {}
void AppUI::onChangePos(double newPos) {}
void AppUI::onChangePause(bool isPaused) {}