#pragma once
#define GLFW_INCLUDE_NONE

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <engine/core/scene.hpp>
#include <engine/graphics/model.hpp>
#include <engine/core/gameObject.hpp>
#include <engine/graphics/meshFactory.hpp>
#include <engine/utils/types.hpp>
#include <engine/graphics/texture.hpp>
#include <engine/components/cameraComponent.hpp>
#include <engine/graphics/renderable/MeshRenderable.hpp>
#include <engine/core/inputManager.hpp>
#include <engine/graphics/renderable/ModelRenderable.hpp>
#include <engine/components/rendererComponent.hpp>
#include <engine/core/lightSystem.hpp>
#include <engine/core/renderSystem.hpp>
#include <engine/graphics/managers/shaderManager.hpp>


class SonicScene : public Scene {
public:
    SonicScene(GLFWwindow* window);
    ~SonicScene() override;

    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

private:
    std::vector<std::unique_ptr<GameObject>> objects;
    std::unique_ptr<Shader> modelShader;
    std::unique_ptr<Shader> lightShader;

    LightSystem lightSystem;
    RenderSystem renderSystem;
    std::unique_ptr<InputManager> input;

    LightNew light;

    int screenWidth = 1280;
    int screenHeight = 720;
    bool isPaused = false;

    //Model sonic;

};