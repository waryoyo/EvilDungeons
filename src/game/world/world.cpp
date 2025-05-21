#include <game/world/world.hpp>
#include <game/world/chunk.hpp>
#include <unordered_set>

void World::generate() {
    loadChunk({ 0,0,0 });
}

void World::render(const RenderContext& context) const {
    for (auto& [pos, chunk] : chunks) {
        chunk->render(context);
    }
}

BlockType World::getBlock(int x, int y, int z) const {
    return BlockType::Air;
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