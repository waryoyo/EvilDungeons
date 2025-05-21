#pragma once

#include <engine/utils/types.hpp>
#include <engine/graphics/binder/emissiveBinder.hpp>
#include <engine/graphics/renderable/IRenderable.hpp>
#include <engine/graphics/meshFactory.hpp>
#include <engine/graphics/texture.hpp>
#include <game/utils/types.hpp>
#include <memory>

static constexpr int SIZE = 32;
static constexpr int block = 16;

class Chunk {
public:

    Chunk(glm::ivec3 position);
    ~Chunk();
    void generate();
    void buildMesh();
    void render(const RenderContext& context);

    BlockType getBlock(glm::ivec3 pos) const;
    const glm::ivec3& getPosition() const;

    // Returns the highest non-air block y for a given x,z column, or -1 if all air
    int getTopBlockY(int x, int z) const;

    //void drawCube(glm::i8vec3 pos, glm::vec3 color);

    //void setBlock(glm::i8vec3 pos, BlockType type);
    //void removeBlock(glm::i8vec3 pos, BlockType type);

private:
    glm::ivec3 position;
    BlockType blocks[SIZE][SIZE][SIZE] = { BlockType::Air };
    std::unique_ptr<IRenderable> mesh;
    std::unique_ptr<EmissiveBinder> emissiveBinder;
    Texture atlas;
    std::vector<Quad> greedyMesh(std::vector<uint32_t>& data);
    void addVerticesIndices(const std::vector<Quad>& quads,
        const glm::ivec3& uVec,
        const glm::ivec3& vVec,
        const glm::vec3& normal,
        const glm::vec3& origin,
        std::vector<Vertex>& vertices,
        std::vector<uint32_t>& indices,
        uint32_t& indexOffset
    );

    bool isAir(glm::ivec3 pos) const;
    GLuint VAO, VBO, EBO;

};
