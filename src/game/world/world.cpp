#include <game/world/world.hpp>
#include <game/world/chunk.hpp>
#include <unordered_set>

void World::generate() {
    loadChunk({ 0,0,0 });
}

void World::render(const RenderContext& context) const {
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


BlockType World::getBlock(int x, int y, int z) const {
    // Convert world coordinates to chunk and local block coordinates
    int chunkX = x >= 0 ? x / CHUNK_SIZE : ((x + 1) / CHUNK_SIZE) - 1;
    int chunkY = y >= 0 ? y / CHUNK_SIZE : ((y + 1) / CHUNK_SIZE) - 1;
    int chunkZ = z >= 0 ? z / CHUNK_SIZE : ((z + 1) / CHUNK_SIZE) - 1;
    glm::ivec3 chunkPos(chunkX, chunkY, chunkZ);
    auto it = chunks.find(chunkPos);
    if (it == chunks.end()) return BlockType::Air;
    int localX = x - chunkX * CHUNK_SIZE;
    int localY = y - chunkY * CHUNK_SIZE;
    int localZ = z - chunkZ * CHUNK_SIZE;
    return it->second->getBlock({localX, localY, localZ});
}

void World::ensureChunksNear(const glm::vec3& playerPos) {
    glm::ivec3 center = {
        static_cast<int>(floor(playerPos.x / CHUNK_SIZE)),
        static_cast<int>(0),
        static_cast<int>(floor(playerPos.z / CHUNK_SIZE))
    };

    
    if (center.x == lastCenter.x && center.z == lastCenter.z)
        return;

    lastCenter = center;
    std::unordered_set<glm::ivec3, Vec3Hash> tokeep;
    tokeep.reserve((2 * HORIZONTAL_RADIUS + 1)* (2 * HORIZONTAL_RADIUS + 1));
    tokeep.insert(center);

    for (int x = -HORIZONTAL_RADIUS; x <= HORIZONTAL_RADIUS; x++) {
        for (int z = -HORIZONTAL_RADIUS; z <= HORIZONTAL_RADIUS; z++) {
            if (x * x + z * z <= HORIZONTAL_RADIUS * HORIZONTAL_RADIUS) {
                tokeep.insert({ center.x + x, 0, center.z + z });
            }
        }
    }
   

    for (auto it = chunks.begin(); it != chunks.end();) {
        if (tokeep.count(it->first) == 0) {
            it = chunks.erase(it);
            continue;
        }
        it++;
    }

    for (const auto& chunkCoord : tokeep) {
        if (chunks.count(chunkCoord) == 0)
            loadChunk(chunkCoord);
    }
  
}

void World::loadChunk(const glm::ivec3& chunkCoords) {
    auto chunk = std::make_unique<Chunk>(chunkCoords);
    chunks[chunkCoords] = std::move(chunk);
    chunks[chunkCoords]->generate();
    chunks[chunkCoords]->buildMesh();
}

void World::unloadChunk(const glm::ivec3& chunkCoords) {
    chunks.erase(chunkCoords);
}

Chunk* World::getCenterChunk() {
    auto it = chunks.find(lastCenter);
    if (it != chunks.end()) {
        return it->second.get();
    }
    return nullptr;
}