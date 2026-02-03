#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

    float fov;
    float nearPlane;
    float farPlane;

public:
    Camera();

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    void setPosition(float x, float y, float z);
    void moveBy(float dx, float dy, float dz);

    glm::vec3 getPosition() const { return position; }
};

#endif