#include "Camera.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

Camera::Camera(glm::vec3 position, float yaw, float pitch)
    : position(position), front(glm::vec3(0.0f, 0.0f, -1.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)), right(glm::vec3(0.0f)), worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
    yaw(yaw), pitch(pitch), movementSpeed(2.5f), mouseSensitivity(0.1f), zoom(45.0f) {
    updateCameraVectors();
}

// Actualizarea matricei de vizualizare pentru mediu 3D
glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

// Procesarea miscarii tastaturii pentru mediu 3D
void Camera::processKeyboard(float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    if (keys[GLFW_KEY_W])
        position += front * velocity;
    if (keys[GLFW_KEY_S])
        position -= front * velocity;
    if (keys[GLFW_KEY_A])
        position -= right * velocity;
    if (keys[GLFW_KEY_D])
        position += right * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset) {
    if (zoom >= 1.0f && zoom <= 45.0f)
        zoom -= yoffset;
    if (zoom <= 1.0f)
        zoom = 1.0f;
    if (zoom >= 45.0f)
        zoom = 45.0f;
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

