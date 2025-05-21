#pragma once

#include <engine_core/utils/types.hpp>
#include <engine_core/graphics/binder/emissiveBinder.hpp>
#include <engine_core/graphics/renderable/IRenderable.hpp>
#include <engine_core/graphics/meshFactory.hpp>
#include <game_engine/utils/types.hpp>
#include <memory>

static constexpr int SIZE = 8;
static constexpr int block = 16;

class Chunk {
public:

    Chunk(glm::ivec3 position);
    void generate();
    void buildMesh();
    void render(const RenderContext& context);

    BlockType getBlock(glm::i8vec3 pos) const;

    //void drawCube(glm::i8vec3 pos, glm::vec3 color);

    //void setBlock(glm::i8vec3 pos, BlockType type);
    //void removeBlock(glm::i8vec3 pos, BlockType type);

private:
    glm::ivec3 position;
    BlockType blocks[SIZE][SIZE][SIZE];
    std::unique_ptr<IRenderable> mesh;
    std::unique_ptr<EmissiveBinder> emissiveBinder;

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

