#include <iostream>
#include <gl/gl_core_3_2.hpp>
#include <mpv/client.h>
#include <mpv/render_gl.h>
#include <SDL.h>

#include "player.hpp"
#include "utils.hpp"

static int one = 1;

static void *getProcAddress(void *fn_ctx, const char *name) {
    return SDL_GL_GetProcAddress(name);
}

const uint64_t PROPERTY_WIDTH = 1;
const uint64_t PROPERTY_HEIGHT = 2;
const uint64_t PROPERTY_DURATION = 3;
const uint64_t PROPERTY_TITLE = 4;
const uint64_t PROPERTY_POS = 5;
const uint64_t PROPERTY_PAUSE = 6;

Player::Player(PlayerStateListener &listener) : stateListener{listener} {
    mpv = mpv_create();
    if (!mpv) {
        throw AppException("MPV", "Player context init failed.");
    }
    if (mpv_initialize(mpv) < 0) {
        throw AppException("MPV", "Player instance init failed.");
    }

    mpv_opengl_init_params openglInitParams;
    openglInitParams.get_proc_address = getProcAddress;

    mpv_render_param initParams[] = {
        {MPV_RENDER_PARAM_API_TYPE, (void *)MPV_RENDER_API_TYPE_OPENGL},
        {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &openglInitParams},
        {MPV_RENDER_PARAM_ADVANCED_CONTROL, &one},
        {MPV_RENDER_PARAM_INVALID, nullptr}};

    if (mpv_render_context_create(&renderContext, mpv, initParams) < 0) {
        throw AppException("MPV", "Failed to initialize mpv GL context");
    }

    onRenderUpdate = SDL_RegisterEvents(1);
    onPlayerEvent = SDL_RegisterEvents(1);
    if (onRenderUpdate == (Uint32)-1 || onPlayerEvent == (Uint32)-1) {
        throw AppException("SDL-MPV", "Could not register events");
    }

    mpv_set_wakeup_callback(mpv, Player::playerEventCallback, this);
    mpv_render_context_set_update_callback(renderContext, Player::renderUpdateCallback, this);

    _width = 10;
    _height = 10;

    gl::GenFramebuffers(1, &fbo);
    gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);

    gl::GenTextures(1, &texture);
    gl::ActiveTexture(gl::TEXTURE0);
    gl::BindTexture(gl::TEXTURE_2D, texture);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::LINEAR);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE);
    gl::TexImage2D(gl::TEXTURE_2D, 0, gl::RGB, _width, _height, 0, gl::RGB, gl::UNSIGNED_BYTE, nullptr);

    gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, texture, 0);
    gl::DrawBuffer(gl::COLOR_ATTACHMENT0);

    if (gl::CheckFramebufferStatus(gl::FRAMEBUFFER) != gl::FRAMEBUFFER_COMPLETE) {
        throw AppException("MPV", "Error creating framebuffer");
    }
    cout << fbo << endl;

    fboSettings.fbo = fbo;
    fboSettings.w = _width;
    fboSettings.h = _height;
    fboSettings.internal_format = gl::RGB8;

    renderParams[0] = {MPV_RENDER_PARAM_OPENGL_FBO, &fboSettings};
    renderParams[1] = {MPV_RENDER_PARAM_FLIP_Y, &one};
    renderParams[2] = {MPV_RENDER_PARAM_INVALID, nullptr};

    mpv_observe_property(mpv, PROPERTY_WIDTH, "width", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, PROPERTY_HEIGHT, "height", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, PROPERTY_DURATION, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, PROPERTY_TITLE, "media-title", MPV_FORMAT_STRING);
    mpv_observe_property(mpv, PROPERTY_POS, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, PROPERTY_PAUSE, "pause", MPV_FORMAT_FLAG);  //core-idle

    mpv_set_option_string(mpv, "reset-on-next-file", "pause");
}

Player::~Player() {
    mpv_render_context_free(renderContext);
    mpv_destroy(mpv);
    if (gl::IsTexture(texture)) {
        gl::DeleteTextures(1, &texture);
    }
    if (gl::IsFramebuffer(fbo)) {
        gl::DeleteFramebuffers(1, &fbo);
    }
}

void Player::bindTexture() {
    gl::BindTexture(gl::TEXTURE_2D, texture);
}

void Player::openFile(string filename) {
    const char *cmd[] = {"loadfile", filename.c_str(), NULL};
    mpv_command_async(mpv, 0, cmd);
}

void Player::pauseToggle() {
    const char *cmd[] = {"cycle", "pause", NULL};
    mpv_command_async(mpv, 0, cmd);
}

void Player::seek(double position) {
    const char *cmd[] = {"seek", "54.56", "absolute-percent+keyframes"};
    //string spos = to_string(position);
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%g", position);
    //cout << "seek " << buffer << endl;
    cmd[1] = buffer;

    mpv_command_async(mpv, 0, cmd);
}

const char *JUMP_LENGTH_FORWARD = "5";
const char *JUMP_LENGTH_BACK = "-5";

void Player::jump(bool forward) {
    const char *cmd[] = {"seek", forward ? JUMP_LENGTH_FORWARD : JUMP_LENGTH_BACK, "relative"};
    mpv_command_async(mpv, 0, cmd);
}

void Player::playerEventCallback(void *instance) {
    SDL_Event event{.type = ((Player *)instance)->onPlayerEvent};
    SDL_PushEvent(&event);
}
void Player::renderUpdateCallback(void *instance) {
    SDL_Event event{.type = ((Player *)instance)->onRenderUpdate};
    SDL_PushEvent(&event);
}

void Player::render() {
    mpv_render_context_render(renderContext, renderParams);
}

bool Player::processMessages(SDL_Event &event) {
    bool redraw = false;
    if (event.type == onRenderUpdate) {
        uint64_t flags = mpv_render_context_update(renderContext);
        if (flags & MPV_RENDER_UPDATE_FRAME)
            redraw = true;
    }
    if (event.type == onPlayerEvent) {
        while (1) {
            mpv_event *mp_event = mpv_wait_event(mpv, 0);
            if (mp_event->event_id == MPV_EVENT_NONE)
                break;
            if (mp_event->event_id == MPV_EVENT_PROPERTY_CHANGE) {
                mpv_event_property *prop = (mpv_event_property *)mp_event->data;
                bool resized = false;
                switch (mp_event->reply_userdata) {
                    case PROPERTY_WIDTH:
                        if (prop->format == MPV_FORMAT_INT64) {
                            int64_t newWidth = *(int64_t *)prop->data;
                            if (newWidth != _width) {
                                _width = newWidth;
                                resized = true;
                            }
                        }
                        break;
                    case PROPERTY_HEIGHT:
                        if (prop->format == MPV_FORMAT_INT64) {
                            int64_t newHeight = *(int64_t *)prop->data;
                            if (newHeight != _height) {
                                _height = newHeight;
                                resized = true;
                            }
                        }
                        break;
                    case PROPERTY_DURATION:
                        if (prop->format == MPV_FORMAT_DOUBLE) {
                            double newDuration = *(double *)prop->data;
                            std::cout << newDuration << std::endl;
                        }
                        break;
                    case PROPERTY_TITLE:
                        if (prop->format == MPV_FORMAT_STRING) {
                            char *newTitle = *(char **)(prop->data);
                            if (newTitle != nullptr) {
                                stateListener.onChangeTitle(newTitle);
                            }
                        }
                        break;
                    case PROPERTY_POS:
                        if (prop->format == MPV_FORMAT_DOUBLE) {
                        }
                        break;
                    case PROPERTY_PAUSE:
                        if (prop->format == MPV_FORMAT_FLAG) {
                            cout << "Pause " << *(int *)(prop->data) << endl;
                        }
                        break;
                }
                if (resized) {
                    resize();
                }
            }
        }
    }
    return redraw;
}

void Player::resize() {
    std::cout << "New size: " << _width << "x" << _height << std::endl;
    fboSettings.w = _width;
    fboSettings.h = _height;
    gl::BindTexture(gl::TEXTURE_2D, texture);
    gl::TexImage2D(gl::TEXTURE_2D, 0, gl::RGB, _width, _height, 0, gl::RGB, gl::UNSIGNED_BYTE, nullptr);
}