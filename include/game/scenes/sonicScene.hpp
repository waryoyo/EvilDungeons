#pragma once

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <engine/core/scene.hpp>
#include <engine/graphics/model.hpp>
#include <engine/core/gameObject.hpp>
#include <engine/components/cameraComponent.hpp>
#include <engine/components/rendererComponent.hpp>


class SonicScene : Scene {
public:
    SonicScene();
    ~SonicScene() override;

    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

private:
    std::vector<std::unique_ptr<GameObject>> objects;

    int screenWidth = 1280;
    int screenHeight = 720;
    //Model sonic;

};