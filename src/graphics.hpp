#pragma once

#include <gl/gl_core_3_2.hpp>

const GLuint VERTEX_POSITION_LOCATION = 0;
const GLuint VERTEX_UV_LOCATION = 1;
const GLuint VERTEX_COLOR_LOCATION = 2;

class Graphics {
   private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    int meshSize;

   public:
    Graphics();
    ~Graphics();
    void loadMesh(GLfloat* vertexData, int vertexCount, GLuint* indexData, int indexCount);
    void drawMesh();
};