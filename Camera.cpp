#include "Camera.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/constants.hpp"

namespace gps {

    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp)
        : cameraPosition(cameraPosition), cameraTarget(cameraTarget), cameraUpDirection(cameraUp) {

        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUp));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));

        pitch = glm::degrees(asin(cameraFrontDirection.y)); 
        yaw = glm::degrees(atan2(cameraFrontDirection.z, cameraFrontDirection.x)); 
    }

    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }

    void Camera::move(MOVE_DIRECTION direction, float speed) {
        switch (direction) {
        case MOVE_FORWARD:
            cameraPosition += speed * cameraFrontDirection;
            break;
        case MOVE_BACKWARD:
            cameraPosition -= speed * cameraFrontDirection;
            break;
        case MOVE_RIGHT:
            cameraPosition += speed * cameraRightDirection;
            break;
        case MOVE_LEFT:
            cameraPosition -= speed * cameraRightDirection;
            break;
        case MOVE_UP:
            cameraPosition += speed * cameraUpDirection; 
            break;
        case MOVE_DOWN:
            cameraPosition -= speed * cameraUpDirection; 
            break;
        }
    }

    void Camera::rotate(float pitchOffset, float yawOffset) {
        pitch += pitchOffset; 
        yaw += yawOffset;     

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        updateCameraVectors();
    }

    void Camera::rotateDirection(ROTATE_DIRECTION direction, float angleStep) {
        switch (direction) {
        case ROTATE_UP:
            rotate(angleStep, 0.0f); 
            break;
        case ROTATE_DOWN:
            rotate(-angleStep, 0.0f);
            break;
        case ROTATE_LEFT:
            rotate(0.0f, -angleStep); 
            break;
        case ROTATE_RIGHT:
            rotate(0.0f, angleStep);
            break;
        }
    }

    void Camera::updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFrontDirection = glm::normalize(front);

        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }

    void Camera::zoom(float amount) {
        cameraPosition += amount * cameraFrontDirection;
    }
}