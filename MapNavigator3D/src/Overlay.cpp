#include "../include/Overlay.hpp"
#include "../include/util.hpp"
#include <iostream>

Overlay::Overlay(const char* pinPath, const char* walkingIconPath, const char* rulerIconPath) {
    iconRenderer.init(pinSize);
    bool ok = iconRenderer.loadTextures(pinPath, walkingIconPath, rulerIconPath);
    if (!ok)
        std::cerr << "Warning: some icon textures failed to load in Overlay ctor.\n";

    textRenderer.init(nullptr);
    loadFont("fonts/arial.ttf", 24);
}

Overlay::~Overlay() {
    Characters.clear();
}

void Overlay::drawWalkIcon(unsigned int shaderProgram, int windowWidth, int windowHeight)
{
    iconRenderer.drawWalkIcon(shaderProgram, windowWidth, windowHeight, walkingMode,
        iconX_px, iconY_px, iconWidth_px, iconHeight_px);
}

void Overlay::drawRulerIcon(unsigned int shaderProgram, int windowWidth, int windowHeight)
{
    iconRenderer.drawRulerIcon(shaderProgram, windowWidth, windowHeight, walkingMode,
        iconX_px, iconY_px, iconWidth_px, iconHeight_px);
}

void Overlay::drawText(const char* text, float x, float y, float scale,
    float r, float g, float b, int winW, int winH)
{
    textRenderer.drawText(text, x, y, scale, r, g, b, winW, winH, nullptr);
}

void Overlay::drawFilledRect(float x, float y, float w, float h,
    float r, float g, float b, float a,
    int screenW, int screenH)
{
    textRenderer.drawFilledRect(x, y, w, h, r, g, b, a, screenW, screenH);
}

bool Overlay::loadFont(const char* path, int fontSize)
{
    std::map<char, ::FTCharacterTR> outMap;
    bool ok = textRenderer.loadFont(path, fontSize, &outMap);

    Characters.clear();
    for (auto& p : outMap) {
        FTCharacter oc;
        oc.TextureID = p.second.TextureID;
        oc.Width = p.second.Width;
        oc.Height = p.second.Height;
        oc.BearingX = p.second.BearingX;
        oc.BearingY = p.second.BearingY;
        oc.Advance = p.second.Advance;
        Characters[p.first] = oc;
    }
    return ok;
}