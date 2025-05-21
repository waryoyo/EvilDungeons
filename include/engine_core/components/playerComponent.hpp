#pragma once

#include "component.hpp"

#include <engine_core/components/cameraComponent.hpp>
#include <engine_core/core/inputManager.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class PlayerComponent : public Component {
    public:
        PlayerComponent(GameObject* owner,
            GLFWwindow* window,
            InputManager* input);
        void onAttach() override;
        void update(float dt) override;
        glm::vec3 getPosition() const;
        void setPosition(const glm::vec3& pos);
    private:
        GLFWwindow* window;
        InputManager* input;
        CameraComponent* camera; 
        glm::vec3 pos;
        glm::vec3 velocity;
        double gravity;
        bool isOnGround;
        float jumpStrength;
        void handleMouse();
        void handleKeyboard(float deltaTime);
};

