#pragma once

#include <engine/utils/types.hpp>
#include <unordered_map>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>

#include <game/world/chunk.hpp>
#include <game/utils/types.hpp>
#include <game/utils/terrainSettings.hpp>

struct Vec3Hash {
    size_t operator()(const glm::ivec3& v) const {
        return std::hash<int>()(v.x) ^ std::hash<int>()(v.y << 1) ^ std::hash<int>()(v.z << 2);
    }
};

// Add frustum structure for culling
struct Frustum {
    glm::vec4 planes[6]; // left, right, bottom, top, near, far
    
    void extractFromMatrix(const glm::mat4& viewProj);
    bool intersectsAABB(const glm::vec3& min, const glm::vec3& max) const;
};

class Chunk;
class World {
public:
	static constexpr int CHUNK_SIZE = 32;

	std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>, Vec3Hash> chunks;

	World();
	~World();

	BlockType getBlock(int x, int y, int z) const;
	void setBlock(int x, int y, int z, BlockType type) const;

	void ensureChunksNear(const glm::vec3& playerPos);

	void generate();
	void render(const RenderContext& context) const;
	// Returns a pointer to the center chunk (the one at lastCenter)
	Chunk* getCenterChunk();

private:
	void loadChunk(const glm::ivec3& chunkCoords);
	void unloadChunk(const glm::ivec3& chunkCoords);
	
	// Frustum culling
	bool isChunkInFrustum(const Chunk* chunk, const Frustum& frustum) const;

	glm::ivec3 lastCenter{ 0, 0, 0 };
	
	// Add chunk loading queue for async processing
	std::queue<glm::ivec3> chunkLoadQueue;
	std::mutex chunkQueueMutex;
	std::thread chunkLoaderThread;
	std::atomic<bool> shouldStopLoading{false};
	
	void chunkLoaderWorker(); // Background thread function
};
