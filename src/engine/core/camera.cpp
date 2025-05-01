#include <engine/core/camera.hpp>

void Camera::handleMouse(GLFWwindow *window, double xPos, double yPos) {
    glm::vec3 direction;
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    float xoffset = xPos - lastX;
    float yoffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    direction.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    direction.y = glm::sin(glm::radians(pitch));
    direction.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

    cameraFront = glm::normalize(direction);
}

void Camera::handleKeyboard(GLFWwindow *window, float deltaTime)
{
    float speed = 5.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        speed *= 3.0f;
    }

    const glm::vec3 cameraRight = glm::normalize(glm::cross(cameraUp, cameraFront));

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        cameraPos += speed * cameraUp;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        cameraPos -= speed * cameraUp;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += speed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= speed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos += speed * cameraRight;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos -= speed * cameraRight;
    }
}

glm::vec3 Camera::getCameraPos() const
{
    return cameraPos;
}

glm::vec3 Camera::getCameraFront() const
{
    return cameraFront;
}

glm::vec3 Camera::getCameraUp() const
{
    return cameraUp;
}

void Camera::setFirstMouse(bool newFirstMouse)
{
    firstMouse = newFirstMouse;
}
