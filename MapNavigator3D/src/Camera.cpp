#include "../include/Camera.hpp"

Camera::Camera()
    : position(0.0f, 10.0f, 0.0f),
    target(0.0f, 0.0f, 0.0f),
    up(0.0f, 1.0f, 0.0f),
    fov(45.0f),
    nearPlane(0.1f),
    farPlane(100.0f)
{
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera::setPosition(float x, float y, float z) {
    position = glm::vec3(x, y, z);
    target = glm::vec3(0.0f, 0.0f, 0.0f);
}

void Camera::moveBy(float dx, float dy, float dz) {
    position.x += dx;
    position.y += dy;
    position.z += dz;

    target.x += dx;
    target.z += dz;
}