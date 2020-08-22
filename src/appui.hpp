#pragma once

#include "player.hpp"

class AppUI : public PlayerStateListener {
   private:
    double duration;
    double position;
    bool paused;
    SDL_Window* _window;

   public:
    AppUI(SDL_Window* window) : duration{0.0}, position{0.0}, paused{true}, _window{window} {};
    void onChangeTitle(string newTitle);
    void onChangeDuration(double newDuration);
    void onChangePos(double newPos);
    void onChangePause(bool isPaused);
};