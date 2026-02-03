#include "../include/Map3D.hpp"
#include "../include/util.hpp"
#include <iostream>

Map3D::Map3D(const char* texturePath, float w, float h)
    : width(w), height(h) {

    int imgW, imgH;
    textureID = loadImageToTexture(texturePath, imgW, imgH);

    std::cout << "Map texture ID: " << textureID << std::endl;
    std::cout << "Image dimensions: " << imgW << "x" << imgH << std::endl;
    std::cout << "Map size in 3D space: " << w << "x" << h << std::endl;

    if (textureID == 0) {
        std::cout << "ERROR: Texture NOT loaded!" << std::endl;
    }

    float vertices[] = {
        -w / 2, 0.0f, -h / 2,    0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
         w / 2, 0.0f, -h / 2,    1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
         w / 2, 0.0f,  h / 2,    1.0f, 1.0f,   0.0f, 1.0f, 0.0f,
        -w / 2, 0.0f,  h / 2,    0.0f, 1.0f,   0.0f, 1.0f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    std::cout << "Map3D VAO created: " << VAO << std::endl;
}

void Map3D::render(unsigned int shaderProgram) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Map3D::~Map3D() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}