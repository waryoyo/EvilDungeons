#include "gameObject.hpp"
#include "gameObject.hpp"
#include <engine/core/gameObject.hpp>

void GameObject::setActive(bool a)
{
	active = a;
}

void GameObject::addComponent(std::unique_ptr<Component> comp)
{
	components.push_back(comp);
}

void GameObject::update(float deltaTime)
{
	for (const auto& component : components) {
		component->update(deltaTime);
	}
}

void GameObject::render(const glm::mat4& VP)
{
	for (const auto& component : components) {
		component->render(VP);
	}
}
