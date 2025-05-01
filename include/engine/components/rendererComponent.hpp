#pragma once
#include "Component.hpp"
#include <engine/graphics/model.hpp>
#include <engine/graphics/renderable/IRenderable.hpp>
#include <engine/graphics/shader.hpp>
#include "transformComponent.hpp"

class RendererComponent : public Component {
public:
    RendererComponent(GameObject* owner,
        const IRenderable& m,
        const Shader& s)
        : Component(owner), model(m), shader(s) {}

    void onAttach() {
        if (!owner->getComponent<TransformComponent>())
            owner->addComponent(std::make_unique<TransformComponent>());
    }

    void render(const glm::mat4& VP) override {

        if (auto* tx = owner->getComponent<TransformComponent>()) {
            shader.use();
            glm::mat4 mvp = VP * tx->getModelMatrix();
            shader.setMat4("u_MVP", mvp);
            model.draw(shader);
        }
    }
private:
    const IRenderable& model;
    Shader shader;
};