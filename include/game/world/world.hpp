#pragma once

#include <engine/utils/types.hpp>
#include <unordered_map>
#include <game/utils/types.hpp>
#include <memory>

struct Vec3Hash {
	size_t operator()(const glm::ivec3& v) const {
		return std::hash<int>()(v.x) ^ std::hash<int>()(v.y << 1) ^ std::hash<int>()(v.z << 2);
	}
};

class Chunk;
class World {
public:
	static constexpr int CHUNK_SIZE = 32;
	static constexpr int HORIZONTAL_RADIUS = 3;

	std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>, Vec3Hash> chunks;

	BlockType getBlock(int x, int y, int z) const;
	void ensureChunksNear(const glm::vec3& playerPos);

	void generate();
	void render(const RenderContext& context) const;
	// Returns a pointer to the center chunk (the one at lastCenter)
	Chunk* getCenterChunk();

private:
	void loadChunk(const glm::ivec3& chunkCoords);
	void unloadChunk(const glm::ivec3& chunkCoords);

	glm::ivec3 lastCenter{ 0, 0, 0 };
};
