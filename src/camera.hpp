#pragma once

#include <glm/glm.hpp>
#include <gl/gl_core_3_2.hpp>
#include "shaders.hpp"

class Camera {
   private:
    float horAngle;
    float vertAngle;
    float zoom;
    glm::mat4 perspMatrix;
    glm::mat4 rotMatrix;
    GLint perspMatrixUniform;
    GLint rotMatrixUniform;

   public:
    Camera(ShaderProgram& program);
    void changeAngles(float dHor, float dVert);
    void changeZoom(float dZoom);
    void changeScreenSize(int width, int height);
    void resetView();
};