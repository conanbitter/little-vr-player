#include "utils.hpp"

#include <sstream>
#include <fstream>

AppException::AppException(const string& component, const string& error, const string& description) {
    stringstream buf;
    buf << "[" << component << "] " << error;
    if (description.length() > 0) {
        buf << " : " << description;
    }
    message = buf.str();
};

const char* AppException::what() const noexcept {
    return message.c_str();
}

void saveToObj(string filename, GLfloat* vertexData, int vertexCount, GLuint* indexData, int indexCount) {
    ofstream file;
    file.open(filename);

    for (int i = 0; i < vertexCount; i++) {
        file << "v "
             << vertexData[i * 5] << " "
             << vertexData[i * 5 + 1] << " "
             << vertexData[i * 5 + 2] << std::endl;
    }
    for (int i = 0; i < vertexCount; i++) {
        file << "vt "
             << vertexData[i * 5 + 3] << " "
             << vertexData[i * 5 + 4] << std::endl;
    }
    for (int i = 0; i < indexCount / 3; i++) {
        file << "f "
             << indexData[i * 3] + 1 << " "
             << indexData[i * 3 + 1] + 1 << " "
             << indexData[i * 3 + 2] + 1 << std::endl;
    }
    file.close();
}