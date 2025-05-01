#pragma once

#include "Component.hpp"
#include "TransformComponent.hpp"
#include <glm/glm.hpp>

class LightComponent : public Component {
public:
    LightComponent(GameObject* owner,
        const glm::vec3& color = glm::vec3(1.0f),
        const glm::vec3& ambient = glm::vec3(0.2f),
        const glm::vec3& diffuse = glm::vec3(0.4f),
        const glm::vec3& specular = glm::vec3(0.8f))
        : Component(owner), color(color), ambient(ambient), diffuse(diffuse), specular(specular)
    {}

    void onAttach() override {
        if (!owner->getComponent<TransformComponent>())
            owner->addComponent(std::make_unique<TransformComponent>(owner));
    }

    glm::vec3 getColor() const { return color; }

private:
    glm::vec3 color;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

};
