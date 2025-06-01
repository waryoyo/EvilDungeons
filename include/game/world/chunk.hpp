#pragma once

#include <engine/utils/types.hpp>
#include <engine/graphics/binder/emissiveBinder.hpp>
#include <engine/graphics/renderable/IRenderable.hpp>
#include <engine/graphics/meshFactory.hpp>
#include <engine/graphics/texture.hpp>
#include <game/utils/types.hpp>
#include <game/utils/terrainSettings.hpp>
#include <memory>
#include <random> 

static constexpr int SIZE = 32;
static constexpr int block = 16;

class Chunk {
public:

    Chunk(glm::ivec3 position);
    ~Chunk();
    void generate();
    void buildMesh();

    void renderOpaque(const RenderContext& context) ;
    void renderTransparent(const RenderContext& context) ;
    BlockType getBlock(glm::ivec3 pos) const;
    void setBlock(const glm::ivec3& pos, BlockType type);

    const glm::ivec3& getPosition() const;

    // Returns the highest non-air block y for a given x,z column, or -1 if all air
    int getTopBlockY(int x, int z) const;

    //void drawCube(glm::i8vec3 pos, glm::vec3 color);

    //void setBlock(glm::i8vec3 pos, BlockType type);
    //void removeBlock(glm::i8vec3 pos, BlockType type);

private:
    glm::ivec3 position;
    BlockType blocks[SIZE][128][SIZE] = { BlockType::Air };
    bool needsMeshUpdate = false;
    std::unique_ptr<IRenderable> mesh;
    std::unique_ptr<IRenderable> opaqueMesh;
    std::unique_ptr<IRenderable> transparentMesh;
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
    bool isTransparent(glm::ivec3 pos) const;

    int baseSeed;
    int mountainSeed;
    // Add these for opaque mesh
    GLuint opaqueVAO = 0;
    GLuint opaqueVBO = 0;
    GLuint opaqueEBO = 0;

    // Add these for transparent mesh
    GLuint transparentVAO = 0;
    GLuint transparentVBO = 0;
    GLuint transparentEBO = 0;
};
