#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "glm/glm.hpp"

namespace gps {

    // Enum for camera movement directions
    enum MOVE_DIRECTION {
        MOVE_FORWARD,
        MOVE_BACKWARD,
        MOVE_RIGHT,
        MOVE_LEFT,
        MOVE_UP,
        MOVE_DOWN
    };

    // Enum for rotation directions
    enum ROTATE_DIRECTION {
        ROTATE_UP,
        ROTATE_DOWN,
        ROTATE_LEFT,
        ROTATE_RIGHT
    };

    class Camera {
    public:
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraUpDirection;
        glm::vec3 cameraFrontDirection;
        glm::vec3 cameraRightDirection;

        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);

        glm::mat4 getViewMatrix();

        void move(MOVE_DIRECTION direction, float speed);

        void rotate(float pitch, float yaw);

        void rotateDirection(ROTATE_DIRECTION direction, float angleStep);

        void updateCameraVectors();

		void zoom(float zoom);

    private:
        float pitch, yaw;
    };
}

#endif // CAMERA_HPP