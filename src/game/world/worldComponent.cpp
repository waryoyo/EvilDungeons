#include <game/world/worldComponent.hpp>
#include <engine/core/gameObject.hpp>
#include <game/world/chunk.hpp>

WorldComponent::WorldComponent(GameObject* owner) : Component(owner)
{
    generate();
}

void WorldComponent::generate() {

    glm::ivec3 pos = glm::ivec3(0, 0, 0);
    auto chunk = std::make_unique<Chunk>(pos);
    chunks[pos] = std::move(chunk);
    chunks[pos]->generate();
    chunks[pos]->buildMesh();
}

void WorldComponent::render(const RenderContext& context) {
    for (auto& [pos, chunk] : chunks) {
        chunk->render(context);
    }
}

