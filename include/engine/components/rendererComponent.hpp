#pragma once

#include "component.hpp"
#include "transformComponent.hpp"

#include <engine/utils/types.hpp>
#include <memory>

class GameObject;
class Shader;
class IRenderable;
class IUniformBinder;
class TransformComponent;

constexpr auto MAX_LIGHTS = 16;

class RendererComponent : public Component {
public:
    RendererComponent(GameObject* owner,
        std::unique_ptr<IRenderable> model,
        Shader* shader,
        const Material& material,
        IUniformBinder* binder,
        bool lightReactive = true);

    void onAttach() override;
    void render(const RenderContext& context) override;

private:
    void setObjectLights(TransformComponent* transform);
    bool GLLogError(const char* function, const char* file, int line);

private:
    std::unique_ptr<IRenderable> model;
    Material material;
    Shader* shader;
    IUniformBinder* binder;
    bool lightReactive;
};
