#pragma once

#include <string>
#include <gl/gl_core_3_2.hpp>

using namespace std;

struct AppException : public std::exception {
    string message;

    AppException(const string& component, const string& error, const string& description = "");

    const char* what() const noexcept override;
};

void saveToObj(string filename, GLfloat* vertexData, int vertexCount, GLuint* indexData, int indexCount);
void createDome(float r);