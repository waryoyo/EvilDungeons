#include <engine/components/lightComponent.hpp>
#include <engine/components/TransformComponent.hpp>

#include <engine/core/scene.hpp>

#include <engine/core/gameObject.hpp>

LightComponent::LightComponent(GameObject* owner, LightNew& light)
    : Component(owner), light(light) {}

void LightComponent::onAttach() {
    if (!owner->getComponent<TransformComponent>())
        owner->addComponent(std::make_unique<TransformComponent>(owner));

    const auto lightSystem = owner->getScene()->getLightSystem();
    lightSystem->setDirty();
}

LightNew LightComponent::getLight() const {
    return light;
}
