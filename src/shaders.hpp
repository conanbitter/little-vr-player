#pragma once

#include <gl/gl_core_3_2.hpp>
#include <string>

using namespace std;

class ShaderProgram {
   private:
    static string getShaderLog(GLuint shader);
    static string getProgramLog(GLuint shaderProgram);

    static GLuint compileShader(string source, GLenum shaderType);

    GLuint program;

   public:
    ShaderProgram(string vertexShader, string fragmentShader);
    ~ShaderProgram();
    void bind();

    operator GLuint() const {
        return program;
    };
};