#pragma once

#include <gl/gl_core_3_2.hpp>
#include <string>

using namespace std;

class Texture {
   private:
    int width;
    int height;
    GLuint handle;

   public:
    Texture();
    ~Texture();
    void loadFromFile(string filename);
    void loadFromMemory(int width, int height, const void* data);
    void bind();
    void getSize(int& w, int& h) {
        w = width;
        h = height;
    }
};