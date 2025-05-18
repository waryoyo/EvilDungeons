#include <engine/components/PlayerComponent.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <engine/core/gameObject.hpp>

PlayerComponent::PlayerComponent(GameObject* owner,
    GLFWwindow* window,
    InputManager* input)
    : Component(owner)
    , input(input)
    , pos(glm::vec3(0.0f,0.0f,3.0f))
    , gravity(9.8f)
    , isOnGround(true)
    , jumpStrength(8.0f)
    , velocity(glm::vec3(0.0f))
{}

void PlayerComponent::onAttach() {
    return;
}

void PlayerComponent::update(float dt) {
    handleKeyboard(dt);
}

glm::vec3 PlayerComponent::getPosition() const {
    return pos;
}

void PlayerComponent::setPosition(const glm::vec3& pos) {
    this->pos = pos;
}

void PlayerComponent::handleMouse() {
    return;
}

void PlayerComponent::handleKeyboard(float deltaTime)
{
    camera = owner->getComponent<CameraComponent>();
    float speed = 5.0f * deltaTime;
    if (input->isKeyDown(GLFW_KEY_LEFT_CONTROL)) {
        speed *= 2.5f;
    }

    glm::vec3 direction(0.0f);
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);

    if (camera) {
        camera->setPosition(pos + glm::vec3(0.0f, 1.7f, 0.0f));
        forward = glm::normalize(glm::vec3(camera->getFront().x, 0.0f, camera->getFront().z));
        right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    }

    if (input->isKeyDown(GLFW_KEY_W)) {
        direction += forward;
    }
    if (input->isKeyDown(GLFW_KEY_S)) {
        direction -= forward;
    }
    if (input->isKeyDown(GLFW_KEY_D)) {
        direction += right;
    }
    if (input->isKeyDown(GLFW_KEY_A)) {
        direction -= right;
    }

    if (glm::length(direction) > 0.0f) {
        direction = glm::normalize(direction);
        pos += direction * speed;
    }

    if (isOnGround && input->wasKeyPressed(GLFW_KEY_SPACE)) {
        velocity.y = jumpStrength;
        isOnGround = false;
    }

    velocity.y -= gravity * deltaTime;
    pos.y += velocity.y * deltaTime;

    if (pos.y <= 0.0f) {
        pos.y = 0.0f;
        velocity.y = 0.0f;
        isOnGround = true;
    }
}

