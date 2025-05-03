#include <engine/components/cameraComponent.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

CameraComponent::CameraComponent(GameObject* owner,
    GLFWwindow* window,
    float fov,
    float aspect,
    float near,
    float far,
    float sensitivity)
    : Component(owner)
    , fov(fov)
    , aspect(aspect)
    , nearPlane(near)
    , farPlane(far)
    , sensitivity(sensitivity)
    , window(window)
    , yaw(-90.0f)
    , xPos(720)
    , yPos(360)
    , lastX(720)
    , lastY(360)
    , pos(0.0f, 0.0f, 3.0f)
    , pitch(0.0f)
    , front(0.0f, 0.0f, -1.0f)
    , up(0.0f, 1.0f, 0.0f)
    , firstMouse(true)
{}

void CameraComponent::onAttach() {
    if (!owner->getComponent<TransformComponent>()) {
        owner->addComponent(std::make_unique<TransformComponent>(owner));
    }

    double x, y;
    glfwGetCursorPos(window, &x, &y);
    lastX = x;
    lastY = y;
}

void CameraComponent::update(float dt) {
    handleMouse();
}

glm::mat4 CameraComponent::getProjection() const {
    return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

glm::mat4 CameraComponent::getView() const {
    return glm::lookAt(pos, pos + front, up);
}

glm::vec3 CameraComponent::getPosition() const {
    return pos;
}

glm::vec3 CameraComponent::getFront() const {
    return front;
}

glm::vec3 CameraComponent::getUp() const {
    return up;
}

void CameraComponent::handleMouse() {
    glm::vec3 direction;
    glfwGetCursorPos(window, &xPos, &yPos);
    if (firstMouse) {
        lastX = xPos;
        lastY = xPos;
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

    front = glm::normalize(direction);
}