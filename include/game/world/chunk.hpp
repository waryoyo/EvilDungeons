#pragma once

#include <engine/utils/types.hpp>
#include <engine/graphics/binder/emissiveBinder.hpp>
#include <engine/graphics/renderable/IRenderable.hpp>

#include <memory>

static constexpr int SIZE = 8;
static constexpr int block = 16;

enum class BlockType : uint8_t {
    Air,
    Dirt,
    Stone,
};

class Chunk {
public:

    Chunk(glm::ivec3 position);
    void generate();
    void buildMesh(); 
    void render(const RenderContext& context);

    BlockType getBlock(glm::i8vec3 pos) const;

    void drawCube(glm::i8vec3 pos, glm::vec3 color);

    void setBlock(glm::i8vec3 pos, BlockType type);
    void removeBlock(glm::i8vec3 pos, BlockType type);

private:
    glm::ivec3 position;
    BlockType blocks[SIZE][SIZE][SIZE];
    std::unique_ptr<IRenderable> mesh;
    std::unique_ptr<EmissiveBinder> emissiveBinder;

    GLuint VAO, VBO, EBO;

    bool isAir(glm::ivec3 pos) const;
};
