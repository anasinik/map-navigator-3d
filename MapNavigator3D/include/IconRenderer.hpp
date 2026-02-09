#pragma once
#include <GL/glew.h>

class IconRenderer {
public:
    IconRenderer() = default;
    ~IconRenderer();

    void init(float pinSize);
    bool loadTextures(const char* pinPath, const char* walkingIconPath, const char* rulerIconPath);

    void drawWalkIcon(unsigned int shaderProgram, int windowWidth, int windowHeight,
        bool walkingMode, float& iconX_px, float& iconY_px,
        float& iconWidth_px, float& iconHeight_px);

    void drawRulerIcon(unsigned int shaderProgram, int windowWidth, int windowHeight,
        bool walkingMode, float& iconX_px, float& iconY_px,
        float& iconWidth_px, float& iconHeight_px);

private:
    GLuint iconVAO = 0;
    GLuint iconVBO = 0;

    GLuint walkingTexture = 0;
    GLuint rulerTexture = 0;
    GLuint pinTexture = 0;

    float pinSize = 0.05f;
    bool initialized = false;

    void ensureInitialized();
    void drawIcon(unsigned int shaderProgram, GLuint texture,
        float x, float y, float w, float h,
        int screenW, int screenH);
};