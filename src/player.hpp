#pragma once

#include <mpv/client.h>
#include <mpv/render_gl.h>
#include <string>
#include <gl/gl_core_3_2.hpp>

using namespace std;

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

   public:
    Player();
    ~Player();
    void openFile(string filename);
    void render();
    bool processMessages(SDL_Event& event);
    void bindTexture();
};