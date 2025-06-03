#pragma once

#include "baseComponent.hpp"
#include <glm/glm.hpp>
#include <engine/core/inputManager.hpp>
#include <GLFW/glfw3.h>

class PhysicsComponent;
class CameraComponent;
class World;

class EnhancedPlayerComponent : public Component {
public:
    EnhancedPlayerComponent(GameObject* owner);
    
    void onAttach() override;
    void update(float deltaTime) override;
    
    // Integration with physics component
    void updateWithPhysics(float deltaTime, PhysicsComponent& physics, CameraComponent& camera, InputManager* inputManager = nullptr);
    
    // Player state queries
    bool isOnGround() const;
    bool isFlying() const { return isFlying; }
    bool isCrouching() const { return isCrouching; }
    bool isSprinting() const { return isSprinting; }
    
    // Settings
    void setMouseSensitivity(float sensitivity);
    void setMovementSpeeds(float walk, float sprint, float crouch, float fly);
    void setJumpForce(float force);

private:
    // Components we work with
    PhysicsComponent* physicsComponent = nullptr;
    CameraComponent* cameraComponent = nullptr;
    TransformComponent* transformComponent = nullptr;
      // Input handling
    void handleInput(float deltaTime, InputManager* inputManager);
    void updateMovementState();
    void updateCamera(float deltaTime);
    
    // Player state
    bool isCrouching = false;
    bool isSprinting = false;
    bool isFlying = false;
    bool isInWater = false;
    
    // Camera control
    float yaw = -90.0f;
    float pitch = 0.0f;
    float mouseSensitivity = 0.1f;
    
    // Movement settings
    float walkSpeed = 4.317f;
    float sprintSpeed = 5.612f;
    float crouchSpeed = 1.295f;
    float jumpForce = 8.42f;
    float flySpeed = 10.89f;
    float waterSlowdown = 0.8f;
};
