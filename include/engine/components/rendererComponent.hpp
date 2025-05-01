#pragma once
#include "Component.hpp"
#include <engine/graphics/model.hpp>
#include <engine/graphics/shader.hpp>

class RendererComponent : public Component {
public:
    RendererComponent(GameObject* owner,
        const engine::graphics::Model& m,
        const engine::graphics::Shader& s)
        : Component(owner), model(m), shader(s) {}

    void render(const glm::mat4& VP) override {
        shader.use();
        shader.setMat4("u_MVP", VP * owner->getModelMatrix());
        model.draw(shader);
    }
private:
    const Model& model;
    Shader shader;
};