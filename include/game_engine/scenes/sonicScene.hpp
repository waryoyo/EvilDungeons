#pragma once
#define GLFW_INCLUDE_NONE

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <engine_core/core/scene.hpp>
#include <engine_core/graphics/model.hpp>
#include <engine_core/graphics/meshFactory.hpp>
#include <engine_core/utils/types.hpp>
#include <engine_core/graphics/texture.hpp>
#include <engine_core/components/cameraComponent.hpp>
#include <engine_core/graphics/renderable/MeshRenderable.hpp>
#include <engine_core/core/inputManager.hpp>
#include <engine_core/graphics/renderable/ModelRenderable.hpp>
#include <engine_core/components/rendererComponent.hpp>
#include <engine_core/core/lightSystem.hpp>
#include <engine_core/core/renderSystem.hpp>
#include <engine_core/graphics/managers/shaderManager.hpp>
#include <engine_core/components/lightComponent.hpp>

class SonicScene : public Scene {
public:
    SonicScene(GLFWwindow* window);
    ~SonicScene() override;

    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

    LightSystem* getLightSystem() const override;

private:
    std::vector<std::unique_ptr<GameObject>> objects;
    std::unique_ptr<Shader> modelShader;
    std::unique_ptr<Shader> lightShader;

    std::unique_ptr<LightSystem> lightSystem;
    std::unique_ptr<RenderSystem> renderSystem;
    std::unique_ptr<InputManager> input;

    LightNew light;

    int screenWidth = 1280;
    int screenHeight = 720;
    bool isPaused = false;

    //Model sonic;

};
