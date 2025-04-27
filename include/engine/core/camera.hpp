#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera {
    public:
        void handleMouse(GLFWwindow* window ,double xPos ,double yPos);
        void handleKeyboard(GLFWwindow* window, float deltaTime);
        glm::vec3 getCameraPos() const;
        glm::vec3 getCameraFront() const;
        glm::vec3 getCameraUp() const;
        void setFirstMouse(bool firstMouse);

    
    private:
        float yaw = -90.0f, pitch = 0.0f;
        const float sensitivity = 0.1f;
        double lastX = 0.0;
        double lastY = 0.0;
        bool firstMouse = true;
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
};