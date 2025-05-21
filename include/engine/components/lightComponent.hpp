#pragma once

#include "baseComponent.hpp"
#include <memory>
#include <engine/utils/types.hpp>

class GameObject;
class TransformComponent;

class LightComponent : public Component {
public:
    LightComponent(GameObject* owner, LightNew& light);
    void onAttach() override;
    LightNew getLight() const;

private:
    LightNew light;
};
