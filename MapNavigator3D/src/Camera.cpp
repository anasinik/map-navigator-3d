#include "../include/Camera.hpp"

Camera::Camera()
    : position(0.0f, 25.0f, 0.0f),
    direction(glm::normalize(glm::vec3(0.0f, -1.0f, -0.5f))),
    up(0.0f, 1.0f, 0.0f),
    fov(45.0f),
    nearPlane(0.1f),
    farPlane(100.0f)
{
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + direction, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera::setPosition(float x, float y, float z) {
    position = glm::vec3(x, y, z);
}

void Camera::moveXZ(float dx, float dz) {
    position.x += dx;
    position.z += dz;
}
