#include <engine/components/components.hpp> 
#include <engine/core/gameObject.hpp>

void ColliderComponent::update(float deltaTime)
{
	auto transform = owner->getComponent<TransformComponent>();
	glm::vec3 worldPos = transform->getPosition();
	glm::vec3 worldScale = transform->getScale();

	worldMin = worldPos + localMin * worldScale;
	worldMax = worldPos + localMax * worldScale;
}

