#include "textures.hpp"
#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>

#include <gl/gl_core_3_2.hpp>

#include "utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(int width, int height) : _width{width}, _height{height} {
    gl::GenTextures(1, &handle);
    gl::ActiveTexture(gl::TEXTURE0);

    gl::BindTexture(gl::TEXTURE_2D, handle);

    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::LINEAR);

    gl::TexImage2D(gl::TEXTURE_2D, 0, gl::RGB, width, height, 0, gl::RGB, gl::UNSIGNED_BYTE, nullptr);

    gl::GenBuffers(1, &pbo);
    gl::BindBuffer(gl::PIXEL_UNPACK_BUFFER, pbo);
    gl::BufferData(gl::PIXEL_UNPACK_BUFFER, width * height * 3, nullptr, gl::STREAM_DRAW);
}

Texture::~Texture() {
    if (gl::IsTexture(handle)) {
        gl::DeleteTextures(1, &handle);
    }
    if (gl::IsBuffer(pbo)) {
        gl::DeleteBuffers(1, &pbo);
    }
}

void Texture::loadFromMemory(const void* data) {
    EASY_FUNCTION();
    gl::BindBuffer(gl::PIXEL_UNPACK_BUFFER, pbo);
    gl::BindTexture(gl::TEXTURE_2D, handle);
    EASY_BLOCK("Pixel transfer to buffer");
    void* mappedBuffer = gl::MapBufferRange(gl::PIXEL_UNPACK_BUFFER, 0, _width * _height * 3, gl::MAP_WRITE_BIT | gl::MAP_INVALIDATE_BUFFER_BIT);
    memcpy(mappedBuffer, data, _width * _height * 3);
    gl::UnmapBuffer(gl::PIXEL_UNPACK_BUFFER);
    //gl::BufferData(gl::PIXEL_UNPACK_BUFFER, _width * _height * 3, data, gl::STREAM_DRAW);
    EASY_END_BLOCK;
    EASY_BLOCK("Pixel transfer to texture");
    gl::TexSubImage2D(gl::TEXTURE_2D, 0, 0, 0, _width, _height, gl::RED, gl::UNSIGNED_BYTE, nullptr);
    EASY_END_BLOCK;
}

void* Texture::lock() {
    EASY_FUNCTION();
    gl::BindBuffer(gl::PIXEL_UNPACK_BUFFER, pbo);
    gl::BindTexture(gl::TEXTURE_2D, handle);
    return gl::MapBufferRange(gl::PIXEL_UNPACK_BUFFER, 0, _width * _height * 3, gl::MAP_WRITE_BIT | gl::MAP_INVALIDATE_BUFFER_BIT);
}

void Texture::unlock() {
    EASY_FUNCTION();
    gl::UnmapBuffer(gl::PIXEL_UNPACK_BUFFER);
    //gl::BufferData(gl::PIXEL_UNPACK_BUFFER, _width * _height * 3, data, gl::STREAM_DRAW);
    EASY_BLOCK("Pixel transfer to texture");
    gl::TexSubImage2D(gl::TEXTURE_2D, 0, 0, 0, _width, _height, gl::RED, gl::UNSIGNED_BYTE, nullptr);
    EASY_END_BLOCK;
}

void Texture::loadFromFile(string filename) {
    int w, h, c;
    stbi_set_flip_vertically_on_load(1);
    void* data = stbi_load(filename.c_str(), &w, &h, &c, 3);
    if (data == nullptr) {
        throw AppException("STB", "Error loading texture", string(stbi_failure_reason()));
    }

    gl::BindBuffer(gl::PIXEL_UNPACK_BUFFER, 0);
    gl::BindTexture(gl::TEXTURE_2D, handle);
    gl::TexImage2D(gl::TEXTURE_2D, 0, gl::RGB, w, h, 0, gl::RGB, gl::UNSIGNED_BYTE, data);
    _width = w;
    _height = h;

    stbi_image_free(data);
}

void Texture::bind() {
    gl::BindTexture(gl::TEXTURE_2D, handle);
}