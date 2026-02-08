#include "../include/Map3D.hpp"
#include "../include/util.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Map3D::Map3D(const char* texturePath, float w, float h)
    : width(w), height(h), sphereIndicesCount(0) {

    int imgW, imgH;
    textureID = loadImageToTexture(texturePath, imgW, imgH);

    // Map vertices
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

    createPinGeometry();
    createSphereGeometry(0.3f, 20, 20);
    createLineGeometry();
}

void Map3D::createPinGeometry() {
    const int segments = 16;
    const float needleRadius = 0.05f;
    const float needleHeight = 2.0f;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Needle cylinder
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * M_PI * i / segments;
        float x = needleRadius * cos(theta);
        float z = needleRadius * sin(theta);

        // Bottom
        vertices.push_back(x);
        vertices.push_back(0.0f);
        vertices.push_back(z);
        vertices.push_back(cos(theta));
        vertices.push_back(0.0f);
        vertices.push_back(sin(theta));

        // Top
        vertices.push_back(x);
        vertices.push_back(needleHeight);
        vertices.push_back(z);
        vertices.push_back(cos(theta));
        vertices.push_back(0.0f);
        vertices.push_back(sin(theta));
    }

    // Indices for cylinder
    for (int i = 0; i < segments; i++) {
        int bottom1 = i * 2;
        int top1 = i * 2 + 1;
        int bottom2 = (i + 1) * 2;
        int top2 = (i + 1) * 2 + 1;

        indices.push_back(bottom1);
        indices.push_back(bottom2);
        indices.push_back(top1);

        indices.push_back(top1);
        indices.push_back(bottom2);
        indices.push_back(top2);
    }

    glGenVertexArrays(1, &pinVAO);
    glGenBuffers(1, &pinVBO);
    glGenBuffers(1, &pinEBO);

    glBindVertexArray(pinVAO);

    glBindBuffer(GL_ARRAY_BUFFER, pinVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pinEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Map3D::createSphereGeometry(float radius, int sectors, int stacks) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = M_PI / 2 - i * M_PI / stacks;
        float xy = radius * cosf(stackAngle);
        float y = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * 2 * M_PI / sectors;
            float x = xy * cosf(sectorAngle);
            float z = xy * sinf(sectorAngle);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            float nx = x / radius;
            float ny = y / radius;
            float nz = z / radius;
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
        }
    }

    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    sphereIndicesCount = static_cast<int>(indices.size());

    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Map3D::createLineGeometry() {
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 100, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Map3D::render(unsigned int shaderProgram) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Map3D::renderLines(unsigned int shaderProgram, const std::vector<MeasurementPin>& pins,
    const glm::mat4& view, const glm::mat4& projection) {
    if (pins.size() < 2) return;

    glUseProgram(shaderProgram);

    int viewLoc = glGetUniformLocation(shaderProgram, "uView");
    int projLoc = glGetUniformLocation(shaderProgram, "uProjection");
    int colorLoc = glGetUniformLocation(shaderProgram, "uColor");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f); // White lines

    std::vector<float> lineVertices;
    for (size_t i = 0; i < pins.size() - 1; i++) {
        // Start point
        lineVertices.push_back(pins[i].position.x);
        lineVertices.push_back(pins[i].position.y + 0.05f); // Slightly above ground
        lineVertices.push_back(pins[i].position.z);

        // End point
        lineVertices.push_back(pins[i + 1].position.x);
        lineVertices.push_back(pins[i + 1].position.y + 0.05f);
        lineVertices.push_back(pins[i + 1].position.z);
    }

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, lineVertices.size() * sizeof(float), lineVertices.data());

    glLineWidth(3.0f);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lineVertices.size() / 3));
    glBindVertexArray(0);
}

void Map3D::renderPins(unsigned int shaderProgram, const std::vector<MeasurementPin>& pins,
    const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos) {
    glUseProgram(shaderProgram);

    int viewLoc = glGetUniformLocation(shaderProgram, "uView");
    int projLoc = glGetUniformLocation(shaderProgram, "uProjection");
    int modelLoc = glGetUniformLocation(shaderProgram, "uModel");
    int colorLoc = glGetUniformLocation(shaderProgram, "uColor");
    int lightPosLoc = glGetUniformLocation(shaderProgram, "uLightPos");
    int lightColorLoc = glGetUniformLocation(shaderProgram, "uLightColor");
    int viewPosLoc = glGetUniformLocation(shaderProgram, "uViewPos");
    int emissiveLoc = glGetUniformLocation(shaderProgram, "uEmissive");
    int numRedLightsLoc = glGetUniformLocation(shaderProgram, "uNumRedLights");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(lightPosLoc, 0.0f, 20.0f, 0.0f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));

    // Pass red light positions (from spheres)
    int numLights = glm::min(static_cast<int>(pins.size()), 10); // Max 10 lights
    glUniform1i(numRedLightsLoc, numLights);

    for (int i = 0; i < numLights; i++) {
        glm::vec3 lightPos = pins[i].position + glm::vec3(0.0f, 2.0f, 0.0f);
        std::string uniformName = "uRedLightPositions[" + std::to_string(i) + "]";
        int loc = glGetUniformLocation(shaderProgram, uniformName.c_str());
        glUniform3fv(loc, 1, glm::value_ptr(lightPos));
    }

    for (const auto& pin : pins) {
        // Render needle (gray)
        glm::mat4 needleModel = glm::mat4(1.0f);
        needleModel = glm::translate(needleModel, pin.position);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(needleModel));
        glUniform3f(colorLoc, 0.5f, 0.5f, 0.5f);
        glUniform1f(emissiveLoc, 0.0f);

        glBindVertexArray(pinVAO);
        glDrawElements(GL_TRIANGLES, 96, GL_UNSIGNED_INT, 0);

        // Render sphere (red with strong emissive)
        glm::mat4 sphereModel = glm::mat4(1.0f);
        sphereModel = glm::translate(sphereModel, pin.position + glm::vec3(0.0f, 2.0f, 0.0f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(sphereModel));
        glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f);
        glUniform1f(emissiveLoc, 1.5f); // Strong emissive for glow effect

        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES, sphereIndicesCount, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
}

Map3D::~Map3D() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &pinVAO);
    glDeleteBuffers(1, &pinVBO);
    glDeleteBuffers(1, &pinEBO);
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereEBO);
    glDeleteVertexArrays(1, &lineVAO);
    glDeleteBuffers(1, &lineVBO);
}