#include <game/world/worldComponent.hpp>
#include <game/world/chunk.hpp>
#include <engine/core/gameObject.hpp>

WorldComponent::WorldComponent(GameObject* owner) : Component(owner)
{
    generate();
}

void WorldComponent::generate() {

    glm::ivec3 pos = glm::ivec3(0, 0, 0);
    auto chunk = std::make_unique<Chunk>(pos, nullptr);
    chunks[pos] = std::move(chunk);
    chunks[pos]->generate();
    chunks[pos]->buildMesh(Chunk::LODLevel::High);
}

// void WorldComponent::render(const RenderContext& context) {
//     for (auto& [pos, chunk] : chunks) {
//         chunk->render(context);
//     }
// }

void WorldComponent::render(const RenderContext& context) {
    // Render opaque blocks first
    for (const auto& [pos, chunk] : chunks) {
        chunk->renderOpaque(context);
    }

    // Enable blending if not already enabled
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Make sure depth test is ON, but disable depth writes
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    // Optional: sort chunks by distance from camera here for proper transparency rendering

    // Render transparent blocks (water)
    for (const auto& [pos, chunk] : chunks) {
        chunk->renderTransparent(context);
    }

    // Re-enable depth writes after transparent pass
    glDepthMask(GL_TRUE);
}

