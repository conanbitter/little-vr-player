#define SDL_MAIN_HANDLED

#include <SDL.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "utils.hpp"

using namespace std;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

SDL_Window *window;
SDL_GLContext context;

string getSDLError() {
    const char *error = SDL_GetError();
    if (error != nullptr) {
        return string(error);
    } else {
        return "unknown";
    }
}

int main(int argc, char **argv) {
    cout << "Init..." << endl;

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow("Little VR player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        throw AppException("SDL", "Error creating context", getSDLError());
    }

    SDL_Event event;
    bool working = true;
    cout << "Run..." << endl;
    while (working) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    working = false;
                    break;
            }
        }

        SDL_GL_SwapWindow(window);
        SDL_Delay(5);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}