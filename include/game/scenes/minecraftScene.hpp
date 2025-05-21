#pragma once
#define GLFW_INCLUDE_NONE

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
    MinecraftScene(GLFWwindow* window);
    ~MinecraftScene() override;

    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

    LightSystem* getLightSystem() const override;

private:
    std::vector<std::unique_ptr<GameObject>> objects;

    std::unique_ptr<LightSystem> lightSystem;
    std::unique_ptr<CollisionSystem> collisionSystem;
    std::unique_ptr<RenderSystem> renderSystem;
    std::unique_ptr<InputManager> input;

    int screenWidth = 1280;
    int screenHeight = 720;
    bool isPaused = false;
    World world;

};