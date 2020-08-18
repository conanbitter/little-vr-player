#pragma once

#include <gl/gl_core_3_2.hpp>
#include <string>
#include <glm/glm.hpp>

using namespace std;

class ShaderProgram {
   private:
    static string getShaderLog(GLuint shader);
    static string getProgramLog(GLuint shaderProgram);

    static GLuint compileShader(string source, GLenum shaderType);

    GLuint program;

   public:
    ShaderProgram(const string& vertexShader, const string& fragmentShader);
    ~ShaderProgram();
    void bind();

    operator GLuint() const {
        return program;
    };

    GLint getUniformHandle(string name);
    void setUniform(GLint uniform, GLint value);
    void setUniform(GLint uniform, GLfloat value);
    void setUniform(GLint uniform, glm::mat4& matrix);
    void setUniform(string uniform, GLint value);
    void setUniform(string uniform, GLfloat value);
    void setUniform(string uniform, glm::mat4& matrix);
};