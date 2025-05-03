#pragma once

#include "Component.hpp"
#include "TransformComponent.hpp"
#include <engine/core/inputManager.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


class CameraComponent : public Component {
public:
    CameraComponent(GameObject* owner,
        GLFWwindow* window,
        InputManager* input,
        float fov = 45.0f,
        float aspect = 1280.0f / 720.0f,
        float near = 0.1f,
        float far = 50.0f,
        float sensitivity = 0.1f
        );

    void onAttach() override;
    void update(float dt) override;

    glm::mat4 getProjection() const;
    glm::mat4 getView() const;
    glm::vec3 getPosition() const;
    glm::vec3 getFront() const;
    glm::vec3 getUp() const;

private:
    GLFWwindow* window;
    InputManager* input;

    float yaw;
    float pitch;
    float sensitivity;
    float fov;
    float aspect;
    float nearPlane;
    float farPlane;

    double xPos;
    double yPos;

    double lastX;
    double lastY;
    bool firstMouse;

    glm::vec3 front;
    glm::vec3 pos;
    glm::vec3 up;

    void handleMouse();
    void handleKeyboard(float deltaTime);
};