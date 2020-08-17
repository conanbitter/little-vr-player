#include "graphics.hpp"

Graphics::Graphics() : meshSize{0} {
    gl::Enable(gl::BLEND);
    gl::Disable(gl::DEPTH_TEST);
    gl::PixelStorei(gl::UNPACK_ALIGNMENT, 1);

    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);
    gl::GenBuffers(1, &vbo);
    gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
    gl::EnableVertexAttribArray(VERTEX_POSITION_LOCATION);
    gl::VertexAttribPointer(VERTEX_POSITION_LOCATION, 3, gl::FLOAT, false, 4 * 5, nullptr);
    gl::EnableVertexAttribArray(VERTEX_UV_LOCATION);
    gl::VertexAttribPointer(VERTEX_UV_LOCATION, 2, gl::FLOAT, false, 4 * 5, (const void*)(3 * 4));

    gl::GenBuffers(1, &ebo);
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ebo);
}

Graphics::~Graphics() {
    if (gl::IsBuffer(vbo)) {
        gl::DeleteBuffers(1, &vbo);
    }
    if (gl::IsBuffer(ebo)) {
        gl::DeleteBuffers(1, &ebo);
    }
    if (gl::IsVertexArray(vao)) {
        gl::DeleteVertexArrays(1, &vao);
    }
}

void Graphics::loadMesh(GLfloat* vertexData, int vertexCount, GLuint* indexData, int indexCount) {
    gl::BufferData(gl::ARRAY_BUFFER, vertexCount * 4 * 5, vertexData, gl::STATIC_DRAW);
    gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, indexCount * 4, indexData, gl::STATIC_DRAW);
    meshSize = indexCount;
}

void Graphics::drawMesh() {
    gl::BindVertexArray(vao);
    gl::DrawElements(gl::TRIANGLES, meshSize, gl::UNSIGNED_INT, nullptr);
}