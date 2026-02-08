#ifndef MAP3D_HPP
#define MAP3D_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

struct MeasurementPin {
    glm::vec3 position;
    float xNorm, zNorm;
};

class Map3D {
private:
    unsigned int VAO, VBO, EBO;
    unsigned int textureID;
    float width;
    float height;

    // Pin geometry
    unsigned int pinVAO, pinVBO, pinEBO;
    unsigned int sphereVAO, sphereVBO, sphereEBO;
    int sphereIndicesCount;

    // Line geometry
    unsigned int lineVAO, lineVBO;

    void createPinGeometry();
    void createSphereGeometry(float radius, int sectors, int stacks);
    void createLineGeometry();

public:
    Map3D(const char* texturePath, float w, float h);
    ~Map3D();

    void render(unsigned int shaderProgram);
    void renderPins(unsigned int shaderProgram, const std::vector<MeasurementPin>& pins,
        const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos);
    void renderLines(unsigned int shaderProgram, const std::vector<MeasurementPin>& pins,
        const glm::mat4& view, const glm::mat4& projection);

    float getWidth() const { return width; }
    float getHeight() const { return height; }
};

#endif