#pragma once
#define GLFW_INCLUDE_NONE

#include <memory>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <engine/core/scene.hpp>
#include <engine/utils/types.hpp>
#include <engine/core/inputManager.hpp>
#include <engine/components/components.hpp>
#include <engine/core/systems/lightSystem.hpp>
#include <engine/core/systems/renderSystem.hpp>
#include <engine/core/systems/collisionSystem.hpp>
#include <engine/graphics/managers/shaderManager.hpp>

#include <game/world/world.hpp>

class MinecraftScene : public Scene {
public:
    explicit MinecraftScene(GLFWwindow* window);
    ~MinecraftScene() override;

    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

    LightSystem* getLightSystem() const override;

private:
    // Rendering
    void renderSky();
    unsigned int loadCubemap();
    void renderPauseMenu();
    void renderStartMenu();
    bool showPauseMenu = false;
    
    // Collision and movement
    bool isColliding(const glm::vec3& position);
    float calculateGroundDistance(const glm::vec3& position);
    glm::vec3 handleCollision(const glm::vec3& currentPos, const glm::vec3& newPos);

    // Scene objects
    std::vector<std::unique_ptr<GameObject>> objects;
    World world;

    // Systems
    std::unique_ptr<LightSystem> lightSystem;
    std::unique_ptr<CollisionSystem> collisionSystem;
    std::unique_ptr<RenderSystem> renderSystem;
    std::unique_ptr<InputManager> input;

    // Skybox
    std::vector<std::string> faces = {
        "assets/textures/sky/right.jpg", 
        "assets/textures/sky/left.jpg", 
        "assets/textures/sky/top.jpg", 
        "assets/textures/sky/bottom.jpg", 
        "assets/textures/sky/front.jpg", 
        "assets/textures/sky/back.jpg"
    };
    unsigned int skyboxVAO = 0;
    unsigned int skyboxVBO = 0;
    unsigned int cubemapTexture = 0;

    // Camera control
    glm::vec3 cameraBasePosition;
    glm::vec3 prevBobOffsetVec;
    glm::vec3 bobOffsetVec;
    float cameraHeight = 1.8f;
    float cameraRadius = 0.3f;

    // Movement parameters
    bool isFirstTime = true;
    bool isPaused = true;
    bool isMoving = false;
    float bobTime = 0.0f;
    float bobSpeed = 20.0f;
    
    // commented cause it causes collision errors
    // float bobAmount = 0.05f;
    float bobAmount = 0.0f;
    float groundDistance = 0.0f;
    glm::vec3 velocity = glm::vec3(0.0f);  
    bool isOnGround = true;             
    float gravity = -10.8f;            
    float jumpSpeed = 5.0f;
    // Window dimensions
    int screenWidth = 1280;
    int screenHeight = 720;
};