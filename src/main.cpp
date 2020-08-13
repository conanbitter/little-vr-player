#define SDL_MAIN_HANDLED
#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>

#include "appwindow.hpp"
#include "utils.hpp"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

int main(int argc, char **argv) {
    profiler::startListen();
    try {
        EASY_BLOCK("Calculating sum");
        AppWindow window(SCREEN_WIDTH, SCREEN_HEIGHT);
        EASY_END_BLOCK;
        window.run();
    } catch (AppException &e) {
        AppWindow::messageBox(e.message);
    }

    return 0;
}