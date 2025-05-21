#include <engine_core/core/gameObject.hpp>
#include <engine_core/components/component.hpp>
#include <engine_core/core/scene.hpp>

void GameObject::setActive(bool a)
{
	active = a;
}

void GameObject::addComponent(std::unique_ptr<Component> comp)
{
	components.push_back(std::move(comp));
}

void GameObject::update(float deltaTime)
{
	for (const auto& component : components) {
		component->update(deltaTime);
	}
}

void GameObject::render(const RenderContext& context)
{
	for (const auto& component : components) {
		component->render(context);
	}
}

const Scene* GameObject::getScene() const
{
	return owner;
}

