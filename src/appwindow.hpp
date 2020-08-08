#pragma once

#include <SDL.h>

#include <string>

#include "utils.hpp"

class AppWindow {
   private:
    SDL_Window* window;
    SDL_GLContext context;

    static string getSDLError();

   public:
    AppWindow(int width, int height);
    ~AppWindow();
    void run();
    static void messageBox(string message);
};