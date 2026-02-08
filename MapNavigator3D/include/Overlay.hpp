#pragma once
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <map>
#include <vector>
#include "../include/point.hpp"
#include "IconRenderer.hpp"
#include "TextRenderer.hpp"

class Overlay {
public:
    Overlay(const char* pinPath, const char* walkingIconPath, const char* rulerIconPath);
    ~Overlay();

    // UI Drawing
    void drawWalkIcon(unsigned int shaderProgram, int windowWidth, int windowHeight);
    void drawRulerIcon(unsigned int shaderProgram, int windowWidth, int windowHeight);
    void drawText(const char* text, float x, float y, float scale,
        float r, float g, float b, int winW, int winH);
    void drawFilledRect(float x, float y, float w, float h,
        float r, float g, float b, float a,
        int screenW, int screenH);

    // Font
    bool loadFont(const char* path, int fontSize);

    struct FTCharacter {
        GLuint TextureID;
        int Width, Height;
        int BearingX, BearingY;
        long Advance;
    };
    std::map<char, FTCharacter> Characters;

    // Mode management
    void setWalkingMode(bool b) { walkingMode = b; }
    bool isWalkingMode() const { return walkingMode; }

    // Icon position (for click detection)
    float iconX_px = 0.0f;
    float iconY_px = 0.0f;
    float iconWidth_px = 0.0f;
    float iconHeight_px = 0.0f;

private:
    bool walkingMode = false;
    IconRenderer iconRenderer;
    TextRenderer textRenderer;
    float pinSize = 0.05f;
};