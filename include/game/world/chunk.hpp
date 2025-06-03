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

static constexpr int CHUNK_SIZE = 32;
static constexpr int block = 16;

// Forward declaration to avoid circular include
class World;

class Chunk {
public:

    Chunk(glm::ivec3 position, World* world = nullptr);
    ~Chunk();void generate();

    void renderOpaque(const RenderContext& context) ;
    void renderTransparent(const RenderContext& context) ;
    BlockType getBlock(glm::ivec3 pos) const;
    void setBlock(const glm::ivec3& pos, BlockType type);    const glm::ivec3& getPosition() const;

    // Returns the highest non-air block y for a given x,z column, or -1 if all air
    int getTopBlockY(int x, int z) const;
    
    // For async chunk loading
    void markNeedsMeshUpdate() { needsMeshUpdate = true; }

    //void drawCube(glm::i8vec3 pos, glm::vec3 color);

    //void setBlock(glm::i8vec3 pos, BlockType type);
    //void removeBlock(glm::i8vec3 pos, BlockType type);

    // LOD system
    enum class LODLevel {
        High = 0,    // Full detail
        Medium = 1,  // Half resolution
        Low = 2      // Quarter resolution
    };
    
    void buildMesh(LODLevel lod = LODLevel::High);
    void setLODLevel(LODLevel level) { currentLOD = level; }
    LODLevel getLODLevel() const { return currentLOD; }

private:
    glm::ivec3 position;
    World* world; // Reference to world for inter-chunk neighbor queries
    BlockType blocks[CHUNK_SIZE][128][CHUNK_SIZE] = { BlockType::Air };
    bool needsMeshUpdate = false;
    std::unique_ptr<IRenderable> mesh;    std::unique_ptr<IRenderable> opaqueMesh;
    std::unique_ptr<IRenderable> transparentMesh;
    std::unique_ptr<EmissiveBinder> emissiveBinder;

    // Atlas is initialized on main thread during mesh building
    std::unique_ptr<Texture> atlas;
    bool atlasInitialized = false;
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
    
    // Better neighbor checking for face culling
    BlockType getNeighborBlock(const glm::ivec3& pos, const glm::ivec3& offset) const;

    int baseSeed;
    int mountainSeed;
    // Add these for opaque mesh
    GLuint opaqueVAO = 0;
    GLuint opaqueVBO = 0;
    GLuint opaqueEBO = 0;

    // Add these for transparent mesh
    GLuint transparentVAO = 0;
    GLuint transparentVBO = 0;
    GLuint transparentEBO = 0;    LODLevel currentLOD = LODLevel::High;
    
    // Removed problematic LOD methods that were creating visual holes
};
