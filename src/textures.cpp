#include "textures.hpp"
#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>

#include <gl/gl_core_3_2.hpp>

#include "utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() : width{0}, height{0} {
    gl::GenTextures(1, &handle);
    gl::ActiveTexture(gl::TEXTURE0);

    gl::BindTexture(gl::TEXTURE_2D, handle);

    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::LINEAR);
}

Texture::~Texture() {
    if (gl::IsTexture(handle)) {
        gl::DeleteTextures(1, &handle);
    }
}

void Texture::loadFromMemory(int width, int height, const void* data) {
    EASY_FUNCTION();
    EASY_BLOCK("Texture bind");
    this->bind();
    EASY_END_BLOCK;
    if (this->width == 0) {
        //gl::PixelStorei(gl::UNPACK_ALIGNMENT, 1);
        //gl::PixelStorei(gl::UNPACK_ROW_LENGTH, width);
        gl::TexImage2D(gl::TEXTURE_2D, 0, gl::RGB, width, height, 0, gl::RED, gl::UNSIGNED_BYTE, data);
        this->width = width;
        this->height = height;
    } else {
        //gl::TexImage2D(gl::TEXTURE_2D, 0, gl::RGB, width, height, 0, gl::RED, gl::UNSIGNED_BYTE, data);
        //gl::PixelStorei(gl::UNPACK_ALIGNMENT, 1);
        //gl::PixelStorei(gl::UNPACK_ROW_LENGTH, width);
        EASY_BLOCK("Pixel transfer");
        gl::TexSubImage2D(gl::TEXTURE_2D, 0, 0, 0, this->width, this->height, gl::RED, gl::UNSIGNED_BYTE, data);
        EASY_END_BLOCK;
    }
}

void Texture::loadFromFile(string filename) {
    int w, h, c;
    stbi_set_flip_vertically_on_load(1);
    void* data = stbi_load(filename.c_str(), &w, &h, &c, 3);
    if (data == nullptr) {
        throw AppException("STB", "Error loading texture", string(stbi_failure_reason()));
    }

    loadFromMemory(w, h, data);

    stbi_image_free(data);
}

void Texture::bind() {
    gl::BindTexture(gl::TEXTURE_2D, handle);
}