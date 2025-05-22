#include <engine/components/cameraComponent.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <engine/core/gameObject.hpp>

CameraComponent::CameraComponent(GameObject* owner,
    GLFWwindow* window,
    InputManager* input,
    float fov,
    float aspect,
    float near,
    float far,
    float sensitivity)
    : Component(owner)
    , input(input)
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
    , isActive(true)
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
    // handleKeyboard(dt);
}

glm::mat4 CameraComponent::getProjection() const {
    return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

glm::mat4 CameraComponent::getView() const {
    return glm::lookAt(pos, pos + front, up);
}
float CameraComponent::getPlayerEyeHeight() const {
    return playerEyeHeight;
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

bool CameraComponent::getIsActive() const {
    return isActive;
}

void CameraComponent::setIsActive(bool isActive) {
    this->isActive = isActive;
}

void CameraComponent::setPosition(const glm::vec3& pos) {
    this->pos = pos;
}

void CameraComponent::setFirstMouse(bool newFirstMouse){
    firstMouse = newFirstMouse;
}

void CameraComponent::handleMouse() {
    if(isActive){
        glm::vec3 direction;
        glfwGetCursorPos(window, &xPos, &yPos);
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

        front = glm::normalize(direction);
    }
}

// void CameraComponent::handleKeyboard(float deltaTime)
// {
//     float speed = 5.0f * deltaTime;

//     if (input->isKeyDown(GLFW_KEY_LEFT_CONTROL)) {
//         speed *= 3.0f;
//     }

//     const glm::vec3 cameraRight = glm::normalize(glm::cross(up, front));

//     if (input->isKeyDown(GLFW_KEY_SPACE)) {
//         pos += speed * up;
//     }
//     if (input->isKeyDown(GLFW_KEY_LEFT_SHIFT)) {
//         pos -= speed * up;
//     }
//     if (input->isKeyDown(GLFW_KEY_W)) {
//         pos += speed * front;
//     }
//     if (input->isKeyDown(GLFW_KEY_S)) {
//         pos -= speed * front;
//     }
//     if (input->isKeyDown(GLFW_KEY_A)) {
//         pos += speed * cameraRight;
//     }
//     if (input->isKeyDown(GLFW_KEY_D)) {
//         pos -= speed * cameraRight;
//     }
// }