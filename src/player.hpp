#pragma once

#include <mpv/client.h>
#include <mpv/render_gl.h>
#include <string>
#include <gl/gl_core_3_2.hpp>
#include <SDL.h>

using namespace std;

class PlayerStateListener {
   public:
    virtual void onChangeTitle(string newTitle){};
    virtual void onChangeDuration(double newDuration){};
    virtual void onChangePos(double newPos){};
    virtual void onChangePause(bool isPaused){};
};

class Player {
   private:
    mpv_handle* mpv;
    mpv_render_context* renderContext;
    Uint32 onPlayerEvent;
    Uint32 onRenderUpdate;
    static void playerEventCallback(void* instance);
    static void renderUpdateCallback(void* instance);
    GLuint fbo;
    GLuint texture;
    mpv_render_param renderParams[3];
    mpv_opengl_fbo fboSettings;
    int _width;
    int _height;
    void resize();
    PlayerStateListener& stateListener;

   public:
    Player(PlayerStateListener& listener);
    ~Player();
    void openFile(string filename);
    void render();
    bool processMessages(SDL_Event& event);
    void bindTexture();
};