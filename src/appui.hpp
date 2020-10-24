#pragma once

#include "player.hpp"

class AppUI : public PlayerStateListener {
   private:
    double duration;
    double position;
    bool paused;
    SDL_Window* _window;

   public:
    AppUI(SDL_Window* window);
    void onChangeTitle(string newTitle);
    void onChangeDuration(double newDuration);
    void onChangePos(double newPos);
    void onChangePause(bool isPaused);
    void onResize(int newWidth, int newHeight);
};