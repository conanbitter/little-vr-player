#include "shaders.hpp"

#include <gl/gl_core_3_2.hpp>
#include <string>
#include <glm/gtc/type_ptr.hpp>

#include "utils.hpp"

string ShaderProgram::getShaderLog(GLuint shader) {
    if (gl::IsShader(shader)) {
        string message = "";

        int infoLogLength = 0;
        int maxLength = infoLogLength;

        gl::GetShaderiv(shader, gl::INFO_LOG_LENGTH, &maxLength);
        char *infoLog = new char[maxLength];

        gl::GetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
        if (infoLogLength > 0) {
            message = infoLog;
        }

        delete[] infoLog;

        return message;
    } else {
        return "Wrong shader handle";
    }
}

string ShaderProgram::getProgramLog(GLuint shaderProgram) {
    if (gl::IsProgram(shaderProgram)) {
        string message = "";

        int infoLogLength = 0;
        int maxLength = infoLogLength;

        gl::GetProgramiv(shaderProgram, gl::INFO_LOG_LENGTH, &maxLength);
        char *infoLog = new char[maxLength];

        gl::GetProgramInfoLog(shaderProgram, maxLength, &infoLogLength, infoLog);
        if (infoLogLength > 0) {
            message = infoLog;
        }

        delete[] infoLog;
        return message;
    } else {
        return "Wrong shader program handle";
    }
}

GLuint ShaderProgram::compileShader(string source, GLenum shaderType) {
    GLuint shader = gl::CreateShader(shaderType);
    const GLchar *lines[] = {
        source.c_str()};
    gl::ShaderSource(shader, 1, lines, NULL);
    gl::CompileShader(shader);
    GLint isCompiled = false;
    gl::GetShaderiv(shader, gl::COMPILE_STATUS, &isCompiled);
    if (isCompiled != true) {
        throw AppException("OpenGL", "Shader compile error", getShaderLog(shader));
    }
    return shader;
}

ShaderProgram::ShaderProgram(const string &vertexShader, const string &fragmentShader) {
    program = gl::CreateProgram();

    GLuint compiledVertexShader = compileShader(vertexShader, gl::VERTEX_SHADER);
    GLuint compiledFragmentShader = compileShader(fragmentShader, gl::FRAGMENT_SHADER);

    gl::AttachShader(program, compiledVertexShader);
    gl::AttachShader(program, compiledFragmentShader);
    gl::LinkProgram(program);
    GLint result = true;
    gl::GetProgramiv(program, gl::LINK_STATUS, &result);
    if (result != true) {
        throw AppException("OpenGL", "Shader link error", getProgramLog(program));
    }
};

ShaderProgram::~ShaderProgram() {
    if (gl::IsProgram(program)) {
        gl::DeleteProgram(program);
    }
}

void ShaderProgram::bind() {
    gl::UseProgram(program);
}

GLint ShaderProgram::getUniformHandle(string name) {
    return gl::GetUniformLocation(program, name.c_str());
}

void ShaderProgram::setUniform(GLint uniform, GLint value) {
    if (uniform != -1) {
        gl::UseProgram(program);
        gl::Uniform1i(uniform, value);
    }
}

void ShaderProgram::setUniform(GLint uniform, GLfloat value) {
    if (uniform != -1) {
        gl::UseProgram(program);
        gl::Uniform1f(uniform, value);
    }
}

void ShaderProgram::setUniform(GLint uniform, glm::mat4 &matrix) {
    if (uniform != -1) {
        gl::UseProgram(program);
        gl::UniformMatrix4fv(uniform, 1, false, glm::value_ptr(matrix));
    }
}

void ShaderProgram::setUniform(string uniform, GLint value) {
    GLint handle = gl::GetUniformLocation(program, uniform.c_str());
    if (handle == -1) {
        throw AppException("OpenGL", "Shader error", "Uniform \"" + uniform + "\" is not found in shader program");
    }
    gl::UseProgram(program);
    gl::Uniform1i(handle, value);
}

void ShaderProgram::setUniform(string uniform, GLfloat value) {
    GLint handle = gl::GetUniformLocation(program, uniform.c_str());
    if (handle == -1) {
        throw AppException("OpenGL", "Shader error", "Uniform \"" + uniform + "\" is not found in shader program");
    }
    gl::UseProgram(program);
    gl::Uniform1f(handle, value);
}

void ShaderProgram::setUniform(string uniform, glm::mat4 &matrix) {
    GLint handle = gl::GetUniformLocation(program, uniform.c_str());
    if (handle == -1) {
        throw AppException("OpenGL", "Shader error", "Uniform \"" + uniform + "\" is not found in shader program");
    }
    gl::UseProgram(program);
    gl::UniformMatrix4fv(handle, 1, false, glm::value_ptr(matrix));
}