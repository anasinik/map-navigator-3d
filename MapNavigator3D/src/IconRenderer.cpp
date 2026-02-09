#include "../include/IconRenderer.hpp"
#include "../include/util.hpp"
#include <iostream>

void IconRenderer::ensureInitialized() {
    if (initialized) return;

    glGenVertexArrays(1, &iconVAO);
    glGenBuffers(1, &iconVBO);

    glBindVertexArray(iconVAO);
    glBindBuffer(GL_ARRAY_BUFFER, iconVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    initialized = true;
}

void IconRenderer::init(float pinSz) {
    pinSize = pinSz;
    ensureInitialized();
}

IconRenderer::~IconRenderer() {
    if (iconVAO) glDeleteVertexArrays(1, &iconVAO);
    if (iconVBO) glDeleteBuffers(1, &iconVBO);
    if (walkingTexture) glDeleteTextures(1, &walkingTexture);
    if (rulerTexture) glDeleteTextures(1, &rulerTexture);
    if (pinTexture) glDeleteTextures(1, &pinTexture);
}

bool IconRenderer::loadTextures(const char* pinPath, const char* walkingIconPath, const char* rulerIconPath) {
    ensureInitialized();

    bool allOk = true;
    int w, h;

    pinTexture = loadImageToTexture(pinPath, w, h);
    if (!pinTexture) {
        std::cerr << "Failed to load pin texture: " << pinPath << "\n";
        allOk = false;
    }
    else {
        std::cout << "Pin texture loaded: " << w << "x" << h << "\n";
    }

    walkingTexture = loadImageToTexture(walkingIconPath, w, h);
    if (!walkingTexture) {
        std::cerr << "Failed to load walking icon: " << walkingIconPath << "\n";
        allOk = false;
    }
    else {
        std::cout << "Walking icon loaded: " << w << "x" << h << "\n";
    }

    rulerTexture = loadImageToTexture(rulerIconPath, w, h);
    if (!rulerTexture) {
        std::cerr << "Failed to load ruler icon: " << rulerIconPath << "\n";
        allOk = false;
    }
    else {
        std::cout << "Ruler icon loaded: " << w << "x" << h << "\n";
    }

    return allOk;
}

void IconRenderer::drawIcon(unsigned int shaderProgram, GLuint texture,
    float x, float y, float w, float h,
    int screenW, int screenH) {
    if (!texture) return;

    float vertices[6][4] = {
        { x,     y + h, 0.0f, 0.0f },  // top-left
        { x,     y,     0.0f, 1.0f },  // bottom-left
        { x + w, y,     1.0f, 1.0f },  // bottom-right

        { x,     y + h, 0.0f, 0.0f },  // top-left
        { x + w, y,     1.0f, 1.0f },  // bottom-right
        { x + w, y + h, 1.0f, 0.0f }   // top-right
    };

    glUseProgram(shaderProgram);

    float left = 0.0f;
    float right = (float)screenW;
    float bottom = 0.0f;
    float top = (float)screenH;

    float projection[16] = {
        2.0f / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        -(right + left) / (right - left), -(top + bottom) / (top - bottom), 0.0f, 1.0f
    };

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, projection);
    glUniform1i(glGetUniformLocation(shaderProgram, "iconTexture"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(iconVAO);
    glBindBuffer(GL_ARRAY_BUFFER, iconVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void IconRenderer::drawWalkIcon(unsigned int shaderProgram, int windowWidth, int windowHeight,
    bool walkingMode, float& iconX_px, float& iconY_px,
    float& iconWidth_px, float& iconHeight_px) {
    float iconSize = 64.0f;
    float margin = 20.0f;

    float x = windowWidth - iconSize - margin;
    float y = windowHeight - iconSize - margin;

    if (walkingMode) {
        drawIcon(shaderProgram, walkingTexture, x, y, iconSize, iconSize, windowWidth, windowHeight);

        iconX_px = x;
        iconY_px = y;
        iconWidth_px = iconSize;
        iconHeight_px = iconSize;
    }
}

void IconRenderer::drawRulerIcon(unsigned int shaderProgram, int windowWidth, int windowHeight,
    bool walkingMode, float& iconX_px, float& iconY_px,
    float& iconWidth_px, float& iconHeight_px) {
    float iconSize = 64.0f;
    float margin = 20.0f;

    float x = windowWidth - iconSize - margin;
    float y = windowHeight - iconSize - margin;

    if (!walkingMode) {
        drawIcon(shaderProgram, rulerTexture, x, y, iconSize, iconSize, windowWidth, windowHeight);

        iconX_px = x;
        iconY_px = y;
        iconWidth_px = iconSize;
        iconHeight_px = iconSize;
    }
}