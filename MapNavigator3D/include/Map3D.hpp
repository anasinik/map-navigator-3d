#ifndef MAP3D_HPP
#define MAP3D_HPP

#include <GL/glew.h>

class Map3D {
private:
    unsigned int VAO, VBO, EBO;
    unsigned int textureID;

    float width;
    float height;

public:
    Map3D(const char* texturePath, float w, float h);
    ~Map3D();

    void render(unsigned int shaderProgram);

    float getWidth() const { return width; }
    float getHeight() const { return height; }
};

#endif