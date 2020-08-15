#pragma once

#include <gl/gl_core_3_2.hpp>
#include <string>

using namespace std;

class Texture {
   private:
    int _width;
    int _height;
    GLuint handle;
    GLuint pbo;

   public:
    Texture(int width, int height);
    ~Texture();
    void loadFromFile(string filename);
    void loadFromMemory(const void* data);
    void* lock();
    void unlock();
    void bind();
    void getSize(int& width, int& height) {
        width = _width;
        height = _height;
    }
};