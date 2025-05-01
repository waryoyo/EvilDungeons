#pragma once

#include <glm/glm.hpp>
#include <engine/core/gameObject.hpp>

class Component {
public:
	Component(GameObject* owner) : owner(owner) {}
	virtual ~Component() = default;
	virtual void update(float deltaTime) {}
	virtual void render(const glm::mat4& VP) {}

protected:
	GameObject* owner;
};
