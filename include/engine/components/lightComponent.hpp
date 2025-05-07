#pragma once

#include "Component.hpp"
#include "TransformComponent.hpp"

#include <engine/utils/types.hpp>

#include <glm/glm.hpp>

class LightComponent : public Component {
public:
    LightComponent(GameObject* owner,
        const LightNew& light)
        : Component(owner), light(light) {}

    void onAttach() override {
        if (!owner->getComponent<TransformComponent>())
            owner->addComponent(std::make_unique<TransformComponent>(owner));
    }

    LightNew getLight() const { return light; }

private:
    LightNew light;
};
