#include <engine/components/rendererComponent.hpp>

#include <engine/graphics/model.hpp>
#include <engine/graphics/renderable/IRenderable.hpp>
#include <engine/graphics/shader.hpp>
#include <engine/utils/types.hpp>
#include <engine/graphics/binder/IUniformBinder.hpp>
#include <engine/components/cameraComponent.hpp>
#include <engine/core/gameObject.hpp>
#include <engine/core/scene.hpp>
#include <engine/core/systems/lightSystem.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <iostream>

RendererComponent::RendererComponent(GameObject* owner,
    std::unique_ptr<IRenderable> model,
    Shader* shader,
    const Material& material,
    IUniformBinder* binder,
    bool lightReactive)
    : Component(owner),
    model(std::move(model)),
    shader(shader),
    material(material),
    binder(binder),
    lightReactive(lightReactive)
{}

void RendererComponent::onAttach() {
    if (!owner->getComponent<TransformComponent>())
        owner->addComponent(std::make_unique<TransformComponent>(owner));
}

void RendererComponent::render(const RenderContext& context) {
    if (auto* tx = owner->getComponent<TransformComponent>()) {
        shader->use();
        glm::mat4 modelMatrix = tx->getModelMatrix();

        const BinderParams params = BinderParams(shader, modelMatrix, context);
        binder->apply(params);
        if (lightReactive)
            setObjectLights(tx);
        model->draw(shader);

    }
}

void RendererComponent::setObjectLights(TransformComponent* transform) {
    const auto& lights = owner->getScene()->getLightSystem()->getAll();

    int count = 0;
    for (int i = 0; i < lights.size() && count < MAX_LIGHTS; i++) {
        auto light = lights[i]->getLight();

        float dist = glm::distance(transform->getPosition(), light.lightPos);
        if (dist <= light.range) {
            std::string index = "lights[" + std::to_string(count) + "]";
            shader->setVec3(index + ".position", light.lightPos);
            shader->setVec3(index + ".color", light.lightColor);
            shader->setFloat(index + ".range", light.range);
            shader->setVec3(index + ".ambient", light.ambient);
            shader->setVec3(index + ".diffuse", light.diffuse);
            shader->setVec3(index + ".specular", light.specular);
            count++;
        }
    }
}

bool RendererComponent::GLLogError(const char* function, const char* file, int line) {
    bool ok = true;
    while (GLenum err = glGetError()) {
        std::cerr
            << "[OpenGL Error] (0x" << std::hex << err << std::dec << ") "
            << function << " " << file << ":" << line << "\n";
        ok = false;
    }
    return ok;
}
