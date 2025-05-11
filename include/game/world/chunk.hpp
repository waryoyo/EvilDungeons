#pragma once

#include <engine/utils/types.hpp>

#include <memory>


enum class BlockType : uint8_t {
    Air,
    Dirt,
    Stone,
};


static constexpr int SIZE = 16;
class IRenderable;
class Chunk {
public:

    Chunk(glm::ivec3 position);
    void generate();
    void buildMesh(); 
    void render(const RenderContext& context);

    BlockType getBlock(glm::i8vec3 pos) const;
    void setBlock(glm::i8vec3 pos, BlockType type);
    void removeBlock(glm::i8vec3 pos, BlockType type);

private:
    glm::ivec3 position;
    BlockType blocks[SIZE][SIZE][SIZE];
    std::unique_ptr<IRenderable> mesh;
};
