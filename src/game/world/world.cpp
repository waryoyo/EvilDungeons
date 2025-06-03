#include <game/world/world.hpp>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>

World::World() {
    // Start the async chunk loading thread
    chunkLoaderThread = std::thread(&World::chunkLoaderWorker, this);
}

World::~World() {
    // Signal the worker thread to stop
    shouldStopLoading.store(true);
    
    // Wait for the thread to finish
    if (chunkLoaderThread.joinable()) {
        chunkLoaderThread.join();
    }
}

void World::generate() {
    int rangeX = gTerrainSettings.horizontalRadius - 1;
    int rangeY = 0;  // e.g., only generate ground level chunks or modify as needed
    int rangeZ = gTerrainSettings.horizontalRadius - 1;

    for (int x = -rangeX; x <= rangeX; ++x) {
        for (int y = 0; y <= rangeY; ++y) {
            for (int z = -rangeZ; z <= rangeZ; ++z) {
                loadChunk({x, y, z});
            }
        }
    }
}


// Frustum culling implementation
void Frustum::extractFromMatrix(const glm::mat4& viewProj) {
    // Extract frustum planes from view-projection matrix
    const float* m = glm::value_ptr(viewProj);
    
    // Left plane
    planes[0] = glm::vec4(m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12]);
    // Right plane
    planes[1] = glm::vec4(m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12]);
    // Bottom plane
    planes[2] = glm::vec4(m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13]);
    // Top plane
    planes[3] = glm::vec4(m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13]);
    // Near plane
    planes[4] = glm::vec4(m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14]);
    // Far plane
    planes[5] = glm::vec4(m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14]);
    
    // Normalize planes
    for (int i = 0; i < 6; i++) {
        float length = glm::length(glm::vec3(planes[i]));
        planes[i] /= length;
    }
}

bool Frustum::intersectsAABB(const glm::vec3& min, const glm::vec3& max) const {
    for (int i = 0; i < 6; i++) {
        glm::vec3 positive = max;
        glm::vec3 negative = min;
        
        // Get positive vertex (furthest in plane normal direction)
        if (planes[i].x < 0) {
            positive.x = min.x;
            negative.x = max.x;
        }
        if (planes[i].y < 0) {
            positive.y = min.y;
            negative.y = max.y;
        }
        if (planes[i].z < 0) {
            positive.z = min.z;
            negative.z = max.z;
        }
        
        // If positive vertex is behind plane, AABB is outside frustum
        if (glm::dot(glm::vec3(planes[i]), positive) + planes[i].w < 0) {
            return false;
        }
    }
    return true;
}

bool World::isChunkInFrustum(const Chunk* chunk, const Frustum& frustum) const {
    glm::vec3 chunkPos = glm::vec3(chunk->getPosition()) * float(CHUNK_SIZE);
    glm::vec3 chunkMin = chunkPos;
    glm::vec3 chunkMax = chunkPos + glm::vec3(CHUNK_SIZE, 128.0f, CHUNK_SIZE);
    
    return frustum.intersectsAABB(chunkMin, chunkMax);
}

void World::render(const RenderContext& context) const {
    // Calculate view frustum for culling
    const auto& cameraPos = context.cameraData.cameraPos;
    const auto& VP = context.cameraData.VP;
    
    // Extract frustum from view-projection matrix
    Frustum frustum;
    frustum.extractFromMatrix(VP);
    
    // Simple distance-based culling combined with frustum culling
    const float maxRenderDistance = (gTerrainSettings.horizontalRadius + 1) * 32;

    // Collect visible chunks using both distance and frustum culling
    std::vector<Chunk*> visibleOpaqueChunks;
    std::vector<Chunk*> visibleTransparentChunks;
      for (const auto& [pos, chunk] : chunks) {
        // First check frustum culling (cheaper than distance calculation)
        if (!isChunkInFrustum(chunk.get(), frustum)) {
            continue;
        }
        
        // Then check distance culling and assign LOD level
        glm::vec3 chunkCenter = glm::vec3(pos) * float(CHUNK_SIZE) + glm::vec3(CHUNK_SIZE * 0.5f, 64.0f, CHUNK_SIZE * 0.5f);
        float distance = glm::length(cameraPos - chunkCenter);
        
        if (distance <= maxRenderDistance) {
            // Assign LOD level based on distance
            Chunk::LODLevel lodLevel;
            if (distance < 64.0f) {
                lodLevel = Chunk::LODLevel::High;
            } else if (distance < 128.0f) {
                lodLevel = Chunk::LODLevel::Medium;
            } else {
                lodLevel = Chunk::LODLevel::Low;
            }
              // Update LOD if it has changed
            if (chunk->getLODLevel() != lodLevel) {
                chunk->setLODLevel(lodLevel);
                chunk->markNeedsMeshUpdate(); // Mark for rebuild instead of rebuilding immediately
            }
            
            visibleOpaqueChunks.push_back(chunk.get());
            visibleTransparentChunks.push_back(chunk.get());
        }
    }

    // Render opaque blocks first
    for (Chunk* chunk : visibleOpaqueChunks) {
        chunk->renderOpaque(context);
    }

    // Enable blending for transparent blocks
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    // Sort transparent chunks by distance for proper alpha blending
    std::sort(visibleTransparentChunks.begin(), visibleTransparentChunks.end(), 
        [&cameraPos](const Chunk* a, const Chunk* b) {
            glm::vec3 centerA = glm::vec3(a->getPosition()) * float(CHUNK_SIZE) + glm::vec3(CHUNK_SIZE * 0.5f);
            glm::vec3 centerB = glm::vec3(b->getPosition()) * float(CHUNK_SIZE) + glm::vec3(CHUNK_SIZE * 0.5f);
            float distA = glm::length(cameraPos - centerA);
            float distB = glm::length(cameraPos - centerB);
            return distA > distB; // Render farthest first
        });

    // Render transparent blocks
    for (Chunk* chunk : visibleTransparentChunks) {
        chunk->renderTransparent(context);
    }

    glDepthMask(GL_TRUE);
}


BlockType World::getBlock(int x, int y, int z) const {
    // Convert world coordinates to chunk and local block coordinates
    int chunkX = x >= 0 ? x / CHUNK_SIZE : ((x + 1) / CHUNK_SIZE) - 1;
    int chunkY = y >= 0 ? y / 128 : ((y + 1) / 128) - 1;
    int chunkZ = z >= 0 ? z / CHUNK_SIZE : ((z + 1) / CHUNK_SIZE) - 1;
    glm::ivec3 chunkPos(chunkX, chunkY, chunkZ);
    auto it = chunks.find(chunkPos);
    if (it == chunks.end()) return BlockType::Air;
    int localX = x - chunkX * CHUNK_SIZE;
    int localY = y - chunkY * 128;
    int localZ = z - chunkZ * CHUNK_SIZE;
    return it->second->getBlock({localX, localY, localZ});
}

void World::setBlock(int x, int y, int z, BlockType type) const {
    // Convert world coordinates to chunk and local block coordinates
    int chunkX = x >= 0 ? x / CHUNK_SIZE : ((x + 1) / CHUNK_SIZE) - 1;
    int chunkY = y >= 0 ? y / 128 : ((y + 1) / 128) - 1;
    int chunkZ = z >= 0 ? z / CHUNK_SIZE : ((z + 1) / CHUNK_SIZE) - 1;
    glm::ivec3 chunkPos(chunkX, chunkY, chunkZ);
    auto it = chunks.find(chunkPos);
    if (it == chunks.end()) return;
    int localX = x - chunkX * CHUNK_SIZE;
    int localY = y - chunkY * 128;
    int localZ = z - chunkZ * CHUNK_SIZE;
    return it->second->setBlock({localX, localY, localZ}, type);
}

void World::chunkLoaderWorker() {
    while (!shouldStopLoading.load()) {
        glm::ivec3 chunkToLoad;
        bool hasWork = false;
        
        // Check if there's work to do
        {
            std::lock_guard<std::mutex> lock(chunkQueueMutex);
            if (!chunkLoadQueue.empty()) {
                chunkToLoad = chunkLoadQueue.front();
                chunkLoadQueue.pop();
                hasWork = true;
            }
        }
          if (hasWork) {
            // Generate chunk data on background thread
            auto chunk = std::make_unique<Chunk>(chunkToLoad, this);
            chunk->generate();
            
            // Add to main chunk map (this needs to be thread-safe)
            {
                std::lock_guard<std::mutex> lock(chunkQueueMutex);
                // Only add if chunk doesn't already exist (avoid race conditions)
                if (chunks.find(chunkToLoad) == chunks.end()) {
                    chunks[chunkToLoad] = std::move(chunk);
                    // Mark that mesh needs to be built on main thread
                    chunks[chunkToLoad]->markNeedsMeshUpdate();
                }
            }
        } else {
            // No work available, sleep briefly
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void World::ensureChunksNear(const glm::vec3& playerPos) {
    glm::ivec3 center = {
        static_cast<int>(floor(playerPos.x / CHUNK_SIZE)),
        static_cast<int>(0),
        static_cast<int>(floor(playerPos.z / CHUNK_SIZE))
    };

    if (center.x == lastCenter.x && center.z == lastCenter.z)
        return;    lastCenter = center;
    std::unordered_set<glm::ivec3, Vec3Hash> tokeep;
    tokeep.reserve((2 * gTerrainSettings.horizontalRadius + 1) * (2 * gTerrainSettings.horizontalRadius + 1));

    for (int x = -gTerrainSettings.horizontalRadius; x <= gTerrainSettings.horizontalRadius; x++) {
        for (int z = -gTerrainSettings.horizontalRadius; z <= gTerrainSettings.horizontalRadius; z++) {
            if (x * x + z * z <= gTerrainSettings.horizontalRadius * gTerrainSettings.horizontalRadius) {
                tokeep.insert({ center.x + x, 0, center.z + z });
            }
        }
    }

    // Remove chunks that are too far away
    for (auto it = chunks.begin(); it != chunks.end();) {
        if (tokeep.count(it->first) == 0) {
            it = chunks.erase(it);
            continue;
        }
        it++;
    }

    // Queue new chunks for async loading
    for (const auto& chunkCoord : tokeep) {
        if (chunks.count(chunkCoord) == 0) {
            std::lock_guard<std::mutex> lock(chunkQueueMutex);
            chunkLoadQueue.push(chunkCoord);
        }
    }
}

void World::loadChunk(const glm::ivec3& chunkCoords) {
    auto chunk = std::make_unique<Chunk>(chunkCoords, this);
    chunks[chunkCoords] = std::move(chunk);
    chunks[chunkCoords]->generate();
    chunks[chunkCoords]->buildMesh(Chunk::LODLevel::High);
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