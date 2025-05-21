#include <engine_core/core/camera.hpp>

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

void Camera::handleKeyboard(const InputManager& input, float deltaTime)
{
    float speed = 5.0f * deltaTime;

    if (input.isKeyDown(GLFW_KEY_LEFT_CONTROL)) {
        speed *= 3.0f;
    }

    const glm::vec3 cameraRight = glm::normalize(glm::cross(cameraUp, cameraFront));

    if (input.isKeyDown(GLFW_KEY_SPACE)) {
        cameraPos += speed * cameraUp;
    }
    if (input.isKeyDown(GLFW_KEY_LEFT_SHIFT)) {
        cameraPos -= speed * cameraUp;
    }
    if (input.isKeyDown(GLFW_KEY_W)) {
        cameraPos += speed * cameraFront;
    }
    if (input.isKeyDown(GLFW_KEY_S)) {
        cameraPos -= speed * cameraFront;
    }
    if (input.isKeyDown(GLFW_KEY_A)) {
        cameraPos += speed * cameraRight;
    }
    if (input.isKeyDown(GLFW_KEY_D)) {
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

