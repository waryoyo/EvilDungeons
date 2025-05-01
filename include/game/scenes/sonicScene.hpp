#pragma once

#include <engine/core/scene.hpp>
#include <engine/graphics/model.hpp>


class SonicScene : Scene {
public:
    SonicScene();
    ~SonicScene() override;

    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void render() override;

private:
    //Model sonic;

};