#include "appwindow.hpp"

#include <gl/gl_core_3_2.hpp>
#include <string>

#include "graphics.hpp"
#include "shaders.hpp"
#include "textures.hpp"
#include "player.hpp"
#include "camera.hpp"
#include "appui.hpp"

const string vertexShaderCode = R"(
    #version 410

    layout(location = 0) in vec3 vert;
    layout(location = 1) in vec2 vertUV;

    out vec2 fragUV;

    uniform mat4 perspMat;
    uniform mat4 rotMat;

    void main() {
        gl_Position = perspMat * rotMat * vec4(vert.x, vert.y, vert.z, 1.0);
        fragUV = vertUV;
    }
)";

const string fragmentShaderCode = R"(
    #version 410
    uniform sampler2D tex;

    in vec2 fragUV;

    layout(location = 0) out vec4 outputColor;

    void main() {
        outputColor = texture(tex, vec2(fragUV.x/2,fragUV.y));
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

AppWindow::AppWindow(int width, int height) : windowWidth{width}, windowHeight{height} {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow(
        "Little VR player",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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
    Graphics graphics;
    createDome(1.0f, graphics);

    ShaderProgram shader(vertexShaderCode, fragmentShaderCode);
    shader.setUniform("tex", 0);

    Camera camera(shader, windowWidth, windowHeight);

    Texture splash;
    splash.loadFromFile("splash.png");

    AppUI ui(window);

    Player player(ui);
    player.openFile("bbb_sunflower_2160p_60fps_normal.mp4");

    SDL_Event event;
    bool working = true;
    bool lookmode = false;
    int oldx, oldy;
    char* dropped_filedir;
    while (working) {
        bool needRedraw = false;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    working = false;
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
                        needRedraw = true;
                    }
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        windowWidth = event.window.data1;
                        windowHeight = event.window.data2;
                        camera.changeScreenSize(windowWidth, windowHeight);
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        lookmode = !lookmode;
                        SDL_SetRelativeMouseMode(lookmode ? SDL_TRUE : SDL_FALSE);
                        if (!lookmode) {
                            SDL_WarpMouseInWindow(window, oldx, oldy);
                        } else {
                            oldx = event.button.x;
                            oldy = event.button.y;
                        }
                    }
                    /*if (event.button.button == SDL_BUTTON_RIGHT && !lookmode) {
                        double position = (double)event.button.x / windowWidth * 100.0;
                        player.seek(position);
                    }*/
                    break;
                case SDL_MOUSEMOTION:
                    if (lookmode) {
                        camera.changeAngles((float)event.motion.xrel / 500.0f, (float)event.motion.yrel / 500.0f);
                    }
                    if (event.motion.state & SDL_BUTTON_RMASK) {
                        camera.changeAnglesPix(-event.motion.xrel, -event.motion.yrel);
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    camera.changeFov(-1.0f * event.wheel.y);
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            working = false;
                            break;
                        case SDLK_RIGHT:
                            player.jump(true);
                            break;
                        case SDLK_LEFT:
                            player.jump(false);
                            break;
                        case SDLK_SPACE:
                            player.pauseToggle();
                    }
                    break;
                case SDL_DROPFILE:
                    dropped_filedir = event.drop.file;
                    player.openFile(dropped_filedir);
                    camera.resetRot();
                    SDL_free(dropped_filedir);
                    break;
                default:
                    needRedraw = player.processMessages(event) || needRedraw;
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
        gl::Viewport(0, 0, windowWidth, windowHeight);
        gl::Scissor(0, 0, windowWidth, windowHeight);
        //gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);
        graphics.drawMesh();

        SDL_GL_SwapWindow(window);
        SDL_Delay(5);
    }
}

void AppWindow::messageBox(string message) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Application error", message.c_str(), NULL);
}