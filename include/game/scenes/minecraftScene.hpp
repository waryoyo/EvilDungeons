#pragma once
#define GLFW_INCLUDE_NONE

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <engine/core/scene.hpp>
#include <engine/utils/types.hpp>
#include <engine/components/cameraComponent.hpp>
#include <engine/components/playerComponent.hpp>
#include <engine/core/inputManager.hpp>
#include <engine/components/rendererComponent.hpp>
#include <engine/core/lightSystem.hpp>
#include <engine/core/renderSystem.hpp>
#include <engine/graphics/managers/shaderManager.hpp>
#include <engine/components/lightComponent.hpp>

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
    std::unique_ptr<RenderSystem> renderSystem;
    std::unique_ptr<InputManager> input;

    int screenWidth = 1280;
    int screenHeight = 720;
    bool isPaused = false;

};