#pragma once
#define GLFW_INCLUDE_NONE

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <engine_core/core/scene.hpp>
#include <engine_core/utils/types.hpp>
#include <engine_core/components/cameraComponent.hpp>
#include <engine_core/components/playerComponent.hpp>
#include <engine_core/core/inputManager.hpp>
#include <engine_core/components/rendererComponent.hpp>
#include <engine_core/core/lightSystem.hpp>
#include <engine_core/core/renderSystem.hpp>
#include <engine_core/graphics/managers/shaderManager.hpp>
#include <engine_core/components/lightComponent.hpp>

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
