#pragma once

#include <gl/gl_core_3_2.hpp>
#include "shaders.hpp"

class Camera {
   private:
    float horAngle;
    float vertAngle;
    float fov;
    float width;
    float height;
    GLint perspMatrixUniform;
    GLint rotMatrixUniform;
    ShaderProgram& _program;
    void updatePersp();
    void updateRot();
    void resetRot();

   public:
    Camera(ShaderProgram& program, int screenWidth, int screenHeight);
    void changeAngles(float dHor, float dVert);
    void changeFov(float dFov);
    void changeScreenSize(int newWidth, int newHeight);
    void resetView();
};