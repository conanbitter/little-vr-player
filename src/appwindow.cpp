#include "appwindow.hpp"

#include <gl/gl_core_3_2.hpp>
#include <string>

#include "graphics.hpp"
#include "shaders.hpp"
#include "textures.hpp"
#include "video.hpp"
#include "player.hpp"

const string vertexShaderCode = R"(
    #version 410

    layout(location = 0) in vec3 vert;
    layout(location = 1) in vec2 vertUV;

    out vec2 fragUV;

    void main() {
        gl_Position = vec4(vert.x, vert.y, vert.z, 1.0);
        fragUV = vertUV;
    }
)";

const string fragmentShaderCode = R"(
    #version 410
    uniform sampler2D tex;

    in vec2 fragUV;

    layout(location = 0) out vec4 outputColor;

    void main() {
        outputColor = texture(tex, vec2(fragUV.x,fragUV.y));
    }
)";

string AppWindow::getSDLError() {
    const char* error = SDL_GetError();
    if (error != nullptr) {
        return string(error);
    } else {
        return "unknown";
    }
}

AppWindow::AppWindow(int width, int height) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow("Little VR player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        throw AppException("SDL", "Error creating context", getSDLError());
    }

    //SDL_GL_SetSwapInterval(0);

    gl::exts::LoadTest glLoadResult = gl::sys::LoadFunctions();
    if (!glLoadResult) {
        throw AppException("OpenGL", "Error in glLoad");
    }

    gl::ClearColor(0.15f, 0.15f, 0.15f, 1.0f);
}

AppWindow::~AppWindow() {
    if (context) {
        SDL_GL_DeleteContext(context);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

void AppWindow::run() {
    GLfloat quadVertex[] = {
        -0.5,
        -0.5,
        0.0,
        0.0,
        0.0,
        -0.5,
        0.5,
        0.0,
        0.0,
        1.0,
        0.5,
        0.5,
        0.0,
        1.0,
        1.0,
        0.5,
        -0.5,
        0.0,
        1.0,
        0.0,
    };

    GLuint quadIndex[] = {0, 1, 2, 0, 2, 3};

    Graphics graphics;
    //graphics.loadMesh(quadVertex, 4, quadIndex, 6);
    //saveToObj("test.obj", quadVertex, 4, quadIndex, 6);
    createDome(1.0f, graphics);

    ShaderProgram shader(vertexShaderCode, fragmentShaderCode);
    shader.bind();

    Texture splash;
    splash.loadFromFile("splash.png");

    Player player;
    player.openFile("bbb_sunflower_2160p_60fps_normal.mp4");

    SDL_Event event;
    bool working = true;
    while (working) {
        bool needRedraw = false;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    working = false;
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_EXPOSED)
                        needRedraw = true;
                    break;
                default:
                    needRedraw = needRedraw || player.processMessages(event);
            }
        }

        if (needRedraw) {
            player.render();
        }

        //gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
        gl::ClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        gl::Clear(gl::COLOR_BUFFER_BIT);
        shader.bind();
        //splash.bind();
        player.bindTexture();
        gl::Viewport(0, 0, 1280, 720);
        gl::Scissor(0, 0, 1280, 720);
        //gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);
        graphics.drawMesh();

        SDL_GL_SwapWindow(window);
        SDL_Delay(5);
    }
}

void AppWindow::messageBox(string message) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Application error", message.c_str(), NULL);
}