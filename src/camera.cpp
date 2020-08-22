#include <glm/ext.hpp>
#include "camera.hpp"
#include "utils.hpp"

const float DEFAULT_FOV = 45.0f;
const float MAX_FOV = 120.0f;
const float MIN_FOV = 20.0f;

Camera::Camera(ShaderProgram& program, int screenWidth, int screenHeight)
    : _program{program}, width{(float)screenWidth}, height{(float)screenHeight} {
    perspMatrixUniform = program.getUniformHandle("perspMat");
    rotMatrixUniform = program.getUniformHandle("rotMat");

    resetView();
}

void Camera::changeAngles(float dHor, float dVert) {
    horAngle += dHor;
    vertAngle += dVert;
    if (horAngle < -PI / 2) horAngle = -PI / 2;
    if (horAngle > PI / 2) horAngle = PI / 2;
    if (vertAngle < -PI / 2) vertAngle = -PI / 2;
    if (vertAngle > PI / 2) vertAngle = PI / 2;
    updateRot();
}

void Camera::changeFov(float dFov) {
    fov += dFov;
    if (fov < MIN_FOV) fov = MIN_FOV;
    if (fov > MAX_FOV) fov = MAX_FOV;
    updatePersp();
}

void Camera::updatePersp() {
    glm::mat4 perspMatrix = glm::perspectiveFov(glm::radians(fov), width, height, 0.01f, 100.0f);
    _program.setUniform(perspMatrixUniform, perspMatrix);
}

void Camera::updateRot() {
    glm::mat4 rotMatrix(1.0f);
    rotMatrix = glm::rotate(rotMatrix, vertAngle, glm::vec3(1.0f, 0.0f, 0.0f));
    rotMatrix = glm::rotate(rotMatrix, horAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    _program.setUniform(rotMatrixUniform, rotMatrix);
}

void Camera::resetRot() {
    glm::mat4 rotMatrix(1.0f);
    _program.setUniform(rotMatrixUniform, rotMatrix);
}

void Camera::resetView() {
    fov = DEFAULT_FOV;
    horAngle = 0.0f;
    vertAngle = 0.0f;
    updatePersp();
    resetRot();
}

void Camera::changeScreenSize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    updatePersp();
}