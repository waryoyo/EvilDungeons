#pragma once
#include "Component.hpp"
#include "transformComponent.hpp"
#include <memory>
#include <engine/graphics/model.hpp>
#include <engine/graphics/renderable/IRenderable.hpp>
#include <engine/graphics/shader.hpp>
#include <engine/utils/types.hpp>

class RendererComponent : public Component {
public:
    RendererComponent(GameObject* owner,
        std::unique_ptr<IRenderable> model,
        const Shader& shader,
        const Material& material,
        const Light& light)
        : Component(owner), model(std::move(model)), shader(shader), material(material), light(light) {}

    void onAttach() {
        if (!owner->getComponent<TransformComponent>())
            owner->addComponent(std::make_unique<TransformComponent>(owner));
    }

    void render(const glm::mat4& VP) override {
        if (auto* tx = owner->getComponent<TransformComponent>()) {
            shader.use();
            glm::mat4 mvp = VP * tx->getModelMatrix();
            shader.setMat4("u_MVP", mvp);
            model->draw(shader);
        }
    }
private:
    std::unique_ptr<IRenderable> model;
    Material material;
    Light light;
    Shader shader;
};