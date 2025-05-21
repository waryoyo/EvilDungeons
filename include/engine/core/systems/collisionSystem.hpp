#pragma once

#include <engine/components/components.hpp>
#include <engine/utils/types.hpp>

class CollisionSystem {
public:
	void addCollider(ColliderComponent* c);
	void removeCollider(ColliderComponent* c);
	void update(float dt);
private:
	std::vector<ColliderComponent*> colliders;

	bool intersectsAABB(const ColliderComponent& a,
		const ColliderComponent& b,
		CollisionManifold& m);
};