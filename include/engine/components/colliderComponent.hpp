#pragma once

#include "baseComponent.hpp"
#include <functional>

class CollisionManifold;
class ColliderComponent : public Component {
public:
	ColliderComponent(GameObject* owner) : Component(owner) {};
	std::function<void(ColliderComponent*, const CollisionManifold&)> onCollisionCallback;

	void update(float deltaTime) override;

	void setLocalAABB(const glm::vec3& low, const glm::vec3& high) {
		localMin = low; localMax = high;
	}

	const glm::vec3& getMin() const { return worldMin; }
	const glm::vec3& getMax() const { return worldMax; }

	void onCollision(ColliderComponent* other,
		const CollisionManifold& m) const
	{
		if (onCollisionCallback)
			onCollisionCallback(other, m);
	}
private:
	glm::vec3 localMin = { -0.5f,-0.5f,-0.5f };
	glm::vec3 localMax = { 0.5f, 0.5f, 0.5f };
	glm::vec3 worldMin;
	glm::vec3 worldMax;
};