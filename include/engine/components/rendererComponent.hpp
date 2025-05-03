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
        Shader* shader,
        const Material& material,
        const Light& light)
        : Component(owner), model(std::move(model)), shader(shader), material(material), light(light) {}

    void onAttach() {
        if (!owner->getComponent<TransformComponent>())
            owner->addComponent(std::make_unique<TransformComponent>(owner));
    }

    void render(const glm::mat4& VP) override {
        if (auto* tx = owner->getComponent<TransformComponent>()) {
            shader->use();
            glm::mat4 modelMatrix = tx->getModelMatrix();
            glm::mat4 mvp = VP * modelMatrix;
            //shader->setInt("uTex", 0);

            shader->setMat4("uMVP", mvp);
            shader->setMat4("uModel", modelMatrix);

            //shader->setVec3("material.ambient", material.ambient);
            //shader->setVec3("material.diffuse", material.diffuse);
            //shader->setVec3("material.specular", material.specular);
            //shader->setFloat("material.shininess", material.shiniess);
            shader->setFloat("shininess", material.shiniess);

            shader->setVec3("light.ambient", light.ambient);
            shader->setVec3("light.diffuse", light.diffuse);
            shader->setVec3("light.specular", light.specular);

            shader->setVec3("lightColor", { 0.2f, 0.2f, 0.25f });
            shader->setVec3("lightPos", { 0.0f, 3.0f, -10.0f });
            shader->setVec3("cameraPos", { 0.0f, 3.0f, 0.0f });

            model->draw(shader);
        }
    }

    bool GLLogError(const char* function, const char* file, int line) {
        bool ok = true;
        while (GLenum err = glGetError()) {
            std::cerr
                << "[OpenGL Error] (0x" << std::hex << err << std::dec << ") "
                << function << " " << file << ":" << line << "\n";
            ok = false;
        }
        return ok;
    }

private:
    std::unique_ptr<IRenderable> model;
    Material material;
    Light light;
    Shader* shader;

};