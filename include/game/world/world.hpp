#pragma once

#include <engine/utils/types.hpp>
#include <unordered_map>
#include <memory>

class Chunk;
class VoxelWorld {
public:
    void generate();
    void render(const RenderContext& context);

private:
    std::unordered_map<glm::ivec3, std::unique_ptr<Chunk> chunks;
};
