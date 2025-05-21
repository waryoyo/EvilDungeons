#pragma once

#include <engine_core/components/component.hpp>
#include <engine_core/utils/types.hpp>
#include <unordered_map>
#include <memory>

struct Vec3Hash {
    size_t operator()(const glm::ivec3& v) const {
        return std::hash<int>()(v.x) ^ std::hash<int>()(v.y << 1) ^ std::hash<int>()(v.z << 2);
    }
};

class Chunk;
class GameObject;

class WorldComponent : public Component {
public:
    WorldComponent(GameObject* owner);
    void generate();
    void render(const RenderContext& context);

private:
    std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>, Vec3Hash> chunks;
};

