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

    std::unique_ptr<InputManager> input;

    Material material;
    Light light;

    std::unique_ptr<Texture> HTexture;

    int screenWidth = 1280;
    int screenHeight = 720;
    //Model sonic;

};