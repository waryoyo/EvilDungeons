#include <engine/components/cameraComponent.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

CameraComponent::CameraComponent(GameObject* owner,
    float fov,
    float aspect,
    float near,
    float far,
    float sensitivity,
    GLFWwindow* window)
    : Component(owner)
    , fov(fov)
    , aspect(aspect)
    , nearPlane(near)
    , farPlane(far)
    , sensitivity(sensitivity)
    , window(window)
    , yaw(-90.0f)
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
    auto* tx = owner->getComponent<TransformComponent>();
    if (!tx) return glm::mat4(1.0f);
    return glm::lookAt(tx->getPosition(), tx->getPosition() + front, up);
}

glm::vec3 CameraComponent::getPosition() const {
    if (auto* tx = owner->getComponent<TransformComponent>()) {
        return tx->getPosition();
    }
    return glm::vec3(0.0f);
}

glm::vec3 CameraComponent::getFront() const {
    return front;
}

glm::vec3 CameraComponent::getUp() const {
    return up;
}

void CameraComponent::handleMouse() {
    glm::vec3 direction;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

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