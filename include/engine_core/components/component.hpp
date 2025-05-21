#pragma once

#include <glm/glm.hpp>
#include <engine_core/utils/types.hpp>

class GameObject;
class Component {
public:
	Component(GameObject* owner) : owner(owner) {}
	virtual ~Component() = default;

	virtual void onAttach() {}
	virtual void update(float deltaTime) {}
	virtual void render(const RenderContext& context) {}

protected:
	GameObject* owner;
};

