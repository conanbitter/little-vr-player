#include "utils.hpp"

#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cmath>

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
    file << setiosflags(ios::fixed) << std::setprecision(6);

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
        int p1 = indexData[i * 3] + 1;
        int p2 = indexData[i * 3 + 1] + 1;
        int p3 = indexData[i * 3 + 2] + 1;
        file << "f "
             << p1 << "/" << p1 << " "
             << p2 << "/" << p2 << " "
             << p3 << "/" << p3 << std::endl;
    }
    file.close();
}

const float PI = 3.14159265358979f;
const int SEGMENTS_VERTICAL = 32;
const int SEGMENTS_HORISONTAL = 32;

void createDome(float r) {
    GLfloat vertices[(SEGMENTS_HORISONTAL + 1) * (SEGMENTS_VERTICAL + 1) * 5];
    GLuint indices[SEGMENTS_HORISONTAL * SEGMENTS_VERTICAL * 2 * 3];

    int index = 0;
    for (int y = 0; y <= SEGMENTS_VERTICAL; y++) {
        float vertAngle = (float)y * (PI / SEGMENTS_VERTICAL);
        float pz = r * cos(vertAngle);
        float lr = r * sin(vertAngle);

        for (int x = 0; x <= SEGMENTS_HORISONTAL; x++) {
            float horAngle = x * (PI / SEGMENTS_HORISONTAL);
            float px = lr * cos(horAngle);
            float py = lr * sin(horAngle);

            vertices[index] = px;      //-r + r * 2 * x / SEGMENTS_HORISONTAL;
            vertices[index + 1] = py;  //-r + r * 2 * y / SEGMENTS_VERTICAL;
            vertices[index + 2] = pz;  //r;
            vertices[index + 3] = (float)x / SEGMENTS_HORISONTAL;
            vertices[index + 4] = (float)y / SEGMENTS_VERTICAL;
            index += 5;
        }
    }
    index = 0;
    for (int y = 0; y < SEGMENTS_VERTICAL; y++) {
        for (int x = 0; x < SEGMENTS_HORISONTAL; x++) {
            int p1 = (y + 1) * (SEGMENTS_HORISONTAL + 1) + x;
            int p2 = y * (SEGMENTS_HORISONTAL + 1) + x;
            int p3 = y * (SEGMENTS_HORISONTAL + 1) + x + 1;
            int p4 = (y + 1) * (SEGMENTS_HORISONTAL + 1) + x + 1;

            indices[index] = p1;
            indices[index + 1] = p2;
            indices[index + 2] = p3;
            indices[index + 3] = p1;
            indices[index + 4] = p3;
            indices[index + 5] = p4;

            index += 6;
        }
    }
    saveToObj("sphere.obj", vertices, (SEGMENTS_HORISONTAL + 1) * (SEGMENTS_VERTICAL + 1), indices, SEGMENTS_HORISONTAL * SEGMENTS_VERTICAL * 2 * 3);
}