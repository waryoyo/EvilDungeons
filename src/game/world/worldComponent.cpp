#include <game/world/worldComponent.hpp>
#include <engine/core/gameObject.hpp>
#include <game/world/chunk.hpp>

WorldComponent::WorldComponent(GameObject* owner) : Component(owner)
{
    generate();
}

void WorldComponent::generate() {
    for (int x = -1; x <= 1; ++x) {
        for (int z = -1; z <= 1; ++z) {
            glm::ivec3 pos = glm::ivec3(x, 0, z);
            auto chunk = std::make_unique<Chunk>(pos);
            chunk->generate();
            chunk->buildMesh();
            //chunk->buildMesh();
            chunks[pos] = std::move(chunk);
        }
    }
}

void WorldComponent::render(const RenderContext& context) {
    for (auto& [pos, chunk] : chunks) {
        chunk->render(context);
    }
}

