#include <engine/core/systems/collisionSystem.hpp>
#include <algorithm>

void CollisionSystem::addCollider(ColliderComponent* c) {
    colliders.push_back(c);
}

void CollisionSystem::removeCollider(ColliderComponent* c) {
    colliders.erase(std::remove(colliders.begin(), colliders.end(), c), colliders.end());
}

void CollisionSystem::update(float dt) {
    const auto n = colliders.size();
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            auto* a = colliders[i];
            auto* b = colliders[j];

            CollisionManifold cm;
            bool collided = false;

            collided = intersectsAABB(*a, *b, cm);
            if (collided) {
                a->onCollision(b, cm);
                cm.normal = -cm.normal;
                b->onCollision(a, cm);
            }

        }
    }
}

bool CollisionSystem::intersectsAABB(const ColliderComponent& a, const ColliderComponent& b, CollisionManifold& m)
{
    const auto& amin = a.getMin();
    const auto& amax = a.getMax();
    const auto& bmin = b.getMin();
    const auto& bmax = b.getMax();

    glm::vec3 aCenter = (amin + amax) * 0.5f;
    glm::vec3 bCenter = (bmin + bmax) * 0.5f;

    if (amax.x < bmin.x || amin.x > bmax.x) return false;
    if (amax.y < bmin.y || amin.y > bmax.y) return false;
    if (amax.z < bmin.z || amin.z > bmax.z) return false;

    float px = std::min(amax.x - bmin.x, bmax.x - amin.x);
    float py = std::min(amax.y - bmin.y, bmax.y - amin.y);
    float pz = std::min(amax.z - bmin.z, bmax.z - amin.z);

    if (px < py && px < pz) {
        m.penetration = px;
        m.normal = ((aCenter.x < bCenter.x)
            ? glm::vec3(-1, 0, 0) : glm::vec3(1, 0, 0));
    }
    else if (py < pz) {
        m.penetration = py;
        m.normal = ((aCenter.y < bCenter.y)
            ? glm::vec3(0, -1, 0) : glm::vec3(0, 1, 0));
    }
    else {
        m.penetration = pz;
        m.normal = ((aCenter.z < bCenter.z)
            ? glm::vec3(0, 0, -1) : glm::vec3(0, 0, 1));
    }
    return true;
}
