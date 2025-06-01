#include <game/world/chunk.hpp>

#include <engine/graphics/renderable/IRenderable.hpp>
#include <engine/graphics/renderable/MeshRenderable.hpp>

#include <engine/graphics/meshFactory.hpp>
#include <engine/graphics/factories/materialFactory.hpp>
#include <engine/graphics/managers/shaderManager.hpp>
#include <engine/graphics/texture.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <bit>
#include <FastNoiseLite.h>

constexpr float tilePx = 16.0f;

struct Tile { int x, y; };
struct BlockTiles {
    Tile top, bottom, side;
};

static const std::unordered_map<BlockType, BlockTiles> blockTileMap = {
    { BlockType::Dirt,   { {1,0}, {1,0}, {1,0} } },
    { BlockType::Grass,  { {0,0}, {1,0}, {2,0} } },
    { BlockType::Water,  { {3,0}, {3,0}, {3,0} } },
    { BlockType::Sand,   { {0,11}, {0,11}, {0,11} }},
    { BlockType::Stone,   { {4,0}, {4,0}, {4,0} }}
};

const glm::ivec3 faceNormals[6] = {
    { 1,  0,  0},
    {-1,  0,  0},
    { 0,  1,  0},
    { 0, -1,  0},
    { 0,  0,  1},
    { 0,  0, -1},
};

const glm::vec3 faceVertices[6][4] = {
    {{1,0,1}, {1,0,0}, {1,1,0}, {1,1,1}},
    {{0,0,0}, {0,0,1}, {0,1,1}, {0,1,0}},
    {{0,1,0}, {0,1,1}, {1,1,1}, {1,1,0}},
    {{0,0,1}, {0,0,0}, {1,0,0}, {1,0,1}},
    {{0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}},
    {{1,0,0}, {0,0,0}, {0,1,0}, {1,1,0}},
};

const glm::vec2 baseUVs[4] = {
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},
};

const glm::ivec3 directions[6] = {
    { 1, 0, 0 }, {-1, 0, 0 },
    { 0, 1, 0 }, { 0,-1, 0 },
    { 0, 0, 1 }, { 0, 0,-1 }
};

const glm::ivec3 uAxes[6] = {
    { 0, 0, 1 }, { 0, 0, 1 },
    { 1, 0, 0 }, { 1, 0, 0 },
    { 1, 0, 0 }, { 1, 0, 0 }
};

const glm::ivec3 vAxes[6] = {
    { 0, 1, 0 }, { 0, 1, 0 },
    { 0, 0, 1 }, { 0, 0, 1 },
    { 0, 1, 0 }, { 0, 1, 0 }
};

bool Chunk::isAir(glm::ivec3 pos) const {
    if (pos.x < 0 || pos.x >= SIZE || pos.y < 0 || pos.y >= SIZE || pos.z < 0 || pos.z >= SIZE)
        return true; // outside chunk treated as air (or you might want to check neighboring chunks)
    return blocks[pos.x][pos.y][pos.z] == BlockType::Air;
}

bool Chunk::isTransparent(glm::ivec3 pos) const {
    if (pos.x < 0 || pos.x >= SIZE || pos.y < 0 || pos.y >= SIZE || pos.z < 0 || pos.z >= SIZE)
        return true; // or false depending on your world setup
    auto block = blocks[pos.x][pos.y][pos.z];
    return block == BlockType::Air || block == BlockType::Water;
}



Chunk::Chunk(glm::ivec3 position) : position(position), atlas("terrain.png")
{
    std::memset(blocks, 0, sizeof(blocks));
    if (!ShaderManager::Get("worldShader"))
        ShaderManager::Load("worldShader", "voxel/basic.vert", "voxel/basic.frag");

    baseSeed = std::random_device{}();
    mountainSeed = std::random_device{}();
    emissiveBinder = std::make_unique<EmissiveBinder>(glm::vec3(1.0f, 1.0f, 1.0f));
}

Chunk::~Chunk() {
    if (opaqueVAO) {
        glDeleteVertexArrays(1, &opaqueVAO);
        glDeleteBuffers(1, &opaqueVBO);
        glDeleteBuffers(1, &opaqueEBO);
    }
    if (transparentVAO) {
        glDeleteVertexArrays(1, &transparentVAO);
        glDeleteBuffers(1, &transparentVBO);
        glDeleteBuffers(1, &transparentEBO);
    }
}


void Chunk::generate() {
    static FastNoiseLite baseNoise;
    static FastNoiseLite mountainNoise;
    static bool initialized = false;

    if (!initialized) {
        // Base terrain: rolling hills
        baseNoise.SetSeed(baseSeed);
        baseNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        baseNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
        baseNoise.SetFractalOctaves(5);
        baseNoise.SetFractalLacunarity(2.0f);
        baseNoise.SetFractalGain(0.5f);
        baseNoise.SetRotationType3D(FastNoiseLite::RotationType3D_ImproveXZPlanes);

        // Mountain ridges
        mountainNoise.SetSeed(mountainSeed);
        mountainNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        mountainNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);
        mountainNoise.SetFractalOctaves(3);

        initialized = true;
    }

    baseNoise.SetFrequency(gTerrainSettings.noiseFrequency);
    mountainNoise.SetFrequency(gTerrainSettings.mountainFrequency);

    int worldX0 = position.x * SIZE;
    int worldY0 = 0;
    int worldZ0 = position.z * SIZE;
    int waterLevel = gTerrainSettings.waterLevel;

    for (int x = 0; x < SIZE; x++) {
        for (int z = 0; z < SIZE; z++) {
            float worldX = float(worldX0 + x);
            float worldZ = float(worldZ0 + z);

            float baseHeight = baseNoise.GetNoise(worldX, worldZ);
            baseHeight = baseHeight * 0.5f + 0.5f;

            float mountainDetail = 0.0f;
            if (baseHeight > 0.6f) {
                float rawMountainNoise = mountainNoise.GetNoise(worldX, worldZ);
                float ridged = 1.0f - std::abs(rawMountainNoise);
                ridged = std::pow(ridged, 3.0f);
                mountainDetail = ridged * (baseHeight - 0.6f) * 2.5f;
            }

            float combinedHeight = baseHeight + mountainDetail;
            if (combinedHeight > 1.5f) combinedHeight = 1.5f;

            int maxTerrainHeight = gTerrainSettings.maxTerrainHeight;
            int height = int(combinedHeight * maxTerrainHeight);

            for (int y = 0; y < 128; y++) {
                int worldY = worldY0 + y;

                if (worldY > height && worldY <= waterLevel) {
                    blocks[x][y][z] = BlockType::Water;
                }
                else if (worldY > height) {
                    blocks[x][y][z] = BlockType::Air;
                }
                else {
                    bool isMountain = (height >= 50);
                    bool nearWater = (height >= waterLevel - 1) && (height <= waterLevel + 1);

                    if (worldY == height) {
                        if (isMountain)
                            blocks[x][y][z] = BlockType::Stone;
                        else if (nearWater)
                            blocks[x][y][z] = BlockType::Sand;
                        else if (height > waterLevel + 1)
                            blocks[x][y][z] = BlockType::Grass;
                        else
                            blocks[x][y][z] = BlockType::Sand;
                    }
                    else if (worldY >= height - 4) {
                        if (isMountain)
                            blocks[x][y][z] = BlockType::Stone;
                        else if (nearWater)
                            blocks[x][y][z] = BlockType::Sand;
                        else if (height > waterLevel + 1)
                            blocks[x][y][z] = BlockType::Dirt;
                        else
                            blocks[x][y][z] = BlockType::Sand;
                    }
                    else {
                        blocks[x][y][z] = BlockType::Stone;
                    }
                }
            }
        }
    }
}

std::vector<Quad> Chunk::greedyMesh(std::vector<uint32_t>& data) {
    std::vector<Quad> quads;
    if (data[0] != 0)
        std::cout << "hello";

    for (uint32_t row = 0; row < SIZE; row++) {
        uint32_t y = 0;
        while (y < SIZE) {
            y += std::countr_zero(data[row] >> y);
            if (y >= SIZE)
                break;

            uint32_t h = std::countr_one(data[row] >> y);
            uint32_t hMask = (1u << h) - 1;
            uint32_t mask = hMask << y;

            uint32_t w = 1;
            while (row + w < SIZE) {
                uint32_t nextRowBits = (data[row + w] >> y) & hMask;
                if (nextRowBits != hMask) break;

                data[row + w] &= ~mask;
                w++;
            }

            quads.emplace_back(row, y, w, h);
            y += h;
        }
    }

    return quads;
}

void Chunk::addVerticesIndices(const std::vector<Quad>& quads,
    const glm::ivec3& uVec,
    const glm::ivec3& vVec,
    const glm::vec3& normal,
    const glm::vec3& origin,
    std::vector<Vertex>& vertices,
    std::vector<uint32_t>& indices,
    uint32_t& indexOffset)
{
    for (const auto& q : quads) {
        glm::vec3 base = glm::vec3(origin);

        glm::vec3 p0 = base + glm::vec3(uVec * int(q.x) + vVec * int(q.y));
        glm::vec3 p1 = base + glm::vec3(uVec * int(q.x + q.w) + vVec * int(q.y));
        glm::vec3 p2 = base + glm::vec3(uVec * int(q.x + q.w) + vVec * int(q.y + q.h));
        glm::vec3 p3 = base + glm::vec3(uVec * int(q.x) + vVec * int(q.y + q.h));

        glm::vec3 n = glm::normalize(glm::vec3(normal));

        vertices.push_back({ p0, baseUVs[0], n});
        vertices.push_back({ p1, baseUVs[1], n});
        vertices.push_back({ p2, baseUVs[2], n});
        vertices.push_back({ p3, baseUVs[3], n});

        glm::vec3 worldU = glm::vec3(uVec);
        glm::vec3 worldV = glm::vec3(vVec);
        bool isAlreadyCCW = glm::dot(glm::cross(worldU, worldV), normal) > 0.0f;
        
        if (isAlreadyCCW) {
            indices.insert(indices.end(), {
                indexOffset + 0, indexOffset + 1, indexOffset + 2,
                indexOffset + 0, indexOffset + 2, indexOffset + 3
                });
        }
        else {
            indices.insert(indices.end(), {
                indexOffset + 0, indexOffset + 2, indexOffset + 1,
                indexOffset + 0, indexOffset + 3, indexOffset + 2
                });
        }

        indexOffset += 4;
    }
}


void Chunk::buildMesh()
{
    const float invTileU = 16.0f / float(256);
    const float invTileV = 16.0f / float(256);

    // Separate vertex/index arrays for opaque and transparent
    std::vector<Vertex> opaqueVertices;
    std::vector<uint32_t> opaqueIndices;
    uint32_t opaqueIndexOffset = 0;

    std::vector<Vertex> transparentVertices;
    std::vector<uint32_t> transparentIndices;
    uint32_t transparentIndexOffset = 0;

    for (int x = 0; x < SIZE; ++x) {
        for (int y = 0; y < 128; ++y) {
            for (int z = 0; z < SIZE; ++z) {
                glm::ivec3 currentBlockPos(x, y, z);
                BlockType blockType = getBlock(currentBlockPos);
                if (blockType == BlockType::Air) continue;

                const auto& tiles = blockTileMap.at(blockType);

                for (int i = 0; i < 6; ++i) {
                    glm::ivec3 faceNormal = directions[i];
                    glm::ivec3 neighborBlockPos = currentBlockPos + faceNormal;

                    bool currentTransparent = isTransparent(currentBlockPos);
                    bool neighborTransparent = isTransparent(neighborBlockPos);

                    // Don't cull faces between air and water
                    if (!currentTransparent && !neighborTransparent) continue; // opaque-opaque
                    if (currentTransparent && neighborTransparent &&
                        getBlock(currentBlockPos) == getBlock(neighborBlockPos)) continue; // same transparent blocks


                    // Face should be rendered if:
                    // - current opaque and neighbor transparent (air or water)
                    // - current transparent and neighbor opaque (or air)
                    // - or neighbor is outside chunk bounds (treated transparent by your function)

                    Tile tile = (faceNormal.y == +1 ? tiles.top
                        : faceNormal.y == -1 ? tiles.bottom
                        : tiles.side);

                    // Flip the y index for the atlas (assuming 16 tiles tall)
                    int flippedY = 15 - tile.y;
                    glm::vec2 uvOrigin(tile.x * invTileU,
                        flippedY * invTileV);
                    glm::vec2 uvSize(invTileU, invTileV);
                    std::vector<glm::vec2> faceUVs = {
                        uvOrigin + glm::vec2(0.0f,        0.0f),
                        uvOrigin + glm::vec2(uvSize.x,    0.0f),
                        uvOrigin + glm::vec2(uvSize.x, uvSize.y),
                        uvOrigin + glm::vec2(0.0f,     uvSize.y)
                    };

                    glm::vec3 blockWorldOrigin =
                        glm::vec3(position * SIZE) +
                        glm::vec3(currentBlockPos);

                    bool isBlockTransparent = (blockType == BlockType::Water);

                    auto& vertices = isBlockTransparent ? transparentVertices : opaqueVertices;
                    auto& indices = isBlockTransparent ? transparentIndices : opaqueIndices;
                    uint32_t& indexOffset = isBlockTransparent ? transparentIndexOffset : opaqueIndexOffset;

                    for (int v = 0; v < 4; ++v) {
                        vertices.push_back({
                            blockWorldOrigin + faceVertices[i][v],
                            faceUVs[v],
                            glm::vec3(faceNormal)
                            });
                    }

                    indices.insert(indices.end(), {
                        indexOffset + 0, indexOffset + 1, indexOffset + 2,
                        indexOffset + 0, indexOffset + 2, indexOffset + 3
                        });
                    indexOffset += 4;
                }
            }
        }
    }

    // Upload opaque mesh (same as before)
    if (!opaqueVertices.empty()) {
        glCreateVertexArrays(1, &opaqueVAO);
        glCreateBuffers(1, &opaqueVBO);
        glCreateBuffers(1, &opaqueEBO);

        glNamedBufferData(opaqueVBO,
            opaqueVertices.size() * sizeof(Vertex),
            opaqueVertices.data(),
            GL_STATIC_DRAW);

        glNamedBufferData(opaqueEBO,
            opaqueIndices.size() * sizeof(uint32_t),
            opaqueIndices.data(),
            GL_STATIC_DRAW);

        glVertexArrayVertexBuffer(opaqueVAO, 0, opaqueVBO, 0, sizeof(Vertex));

        glVertexArrayAttribFormat(opaqueVAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
        glVertexArrayAttribBinding(opaqueVAO, 0, 0);
        glEnableVertexArrayAttrib(opaqueVAO, 0);

        glVertexArrayAttribFormat(opaqueVAO, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
        glVertexArrayAttribBinding(opaqueVAO, 1, 0);
        glEnableVertexArrayAttrib(opaqueVAO, 1);

        glVertexArrayAttribFormat(opaqueVAO, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
        glVertexArrayAttribBinding(opaqueVAO, 2, 0);
        glEnableVertexArrayAttrib(opaqueVAO, 2);

        glVertexArrayElementBuffer(opaqueVAO, opaqueEBO);

        opaqueMesh = std::make_unique<MeshRenderable>(opaqueVAO, opaqueIndices.size());
    } else {
        opaqueMesh.reset();
    }

    // Upload transparent mesh (same as before)
    if (!transparentVertices.empty()) {
        glCreateVertexArrays(1, &transparentVAO);
        glCreateBuffers(1, &transparentVBO);
        glCreateBuffers(1, &transparentEBO);

        glNamedBufferData(transparentVBO,
            transparentVertices.size() * sizeof(Vertex),
            transparentVertices.data(),
            GL_STATIC_DRAW);

        glNamedBufferData(transparentEBO,
            transparentIndices.size() * sizeof(uint32_t),
            transparentIndices.data(),
            GL_STATIC_DRAW);

        glVertexArrayVertexBuffer(transparentVAO, 0, transparentVBO, 0, sizeof(Vertex));

        glVertexArrayAttribFormat(transparentVAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
        glVertexArrayAttribBinding(transparentVAO, 0, 0);
        glEnableVertexArrayAttrib(transparentVAO, 0);

        glVertexArrayAttribFormat(transparentVAO, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
        glVertexArrayAttribBinding(transparentVAO, 1, 0);
        glEnableVertexArrayAttrib(transparentVAO, 1);

        glVertexArrayAttribFormat(transparentVAO, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
        glVertexArrayAttribBinding(transparentVAO, 2, 0);
        glEnableVertexArrayAttrib(transparentVAO, 2);

        glVertexArrayElementBuffer(transparentVAO, transparentEBO);

        transparentMesh = std::make_unique<MeshRenderable>(transparentVAO, transparentIndices.size());
    } else {
        transparentMesh.reset();
    }
}



void Chunk::renderOpaque(const RenderContext& context) {
    if (needsMeshUpdate) {
        buildMesh();
        needsMeshUpdate = false;
    }

    if (!opaqueMesh || !ShaderManager::Get("worldShader")) return;

    auto shader = ShaderManager::Get("worldShader");
    BinderParams params(shader, glm::mat4(1.0f), context);

    shader->use();
    shader->setInt("texture_diffuse", 0);
    shader->setVec3("uCameraPos", context.cameraData.cameraPos);
    shader->setVec3("uFogColor", glm::vec3(1.0f, 1.0f, 1.0f));
    shader->setFloat("uFogStart", 100.0f);
    shader->setFloat("uFogEnd", 160.0f);

    emissiveBinder->apply(params);
    atlas.bind(0);
    opaqueMesh->draw(shader);
}


void Chunk::renderTransparent(const RenderContext& context) {
    if (needsMeshUpdate) {
        buildMesh();
        needsMeshUpdate = false;
    }

    if (!transparentMesh || !ShaderManager::Get("worldShader")) return;

    auto shader = ShaderManager::Get("worldShader");
    BinderParams params(shader, glm::mat4(1.0f), context);

    shader->use();
    shader->setInt("texture_diffuse", 0);
    shader->setVec3("uCameraPos", context.cameraData.cameraPos);
    shader->setVec3("uFogColor", glm::vec3(1.0f, 1.0f, 1.0f));
    shader->setFloat("uFogStart", 100.0f);
    shader->setFloat("uFogEnd", 160.0f);

    emissiveBinder->apply(params);
    atlas.bind(0);
    transparentMesh->draw(shader);
}



BlockType Chunk::getBlock(glm::ivec3 pos) const
{
    if (pos.x < 0 || pos.x >= SIZE || pos.y < 0 || pos.y >= 128 || pos.z < 0 || pos.z >= SIZE)
        return BlockType::Air;
    return blocks[pos.x][pos.y][pos.z];
}

void Chunk::setBlock(const glm::ivec3& pos, BlockType type) {
    if (pos.x < 0 || pos.x >= SIZE || pos.y < 0 || pos.y >= 128 || pos.z < 0 || pos.z >= SIZE)
        return;

    blocks[pos.x][pos.y][pos.z] = type;

    // Optional: mark chunk as dirty so it can rebuild its mesh
    needsMeshUpdate = true;
}


const glm::ivec3& Chunk::getPosition() const
{
    return position;
}

int Chunk::getTopBlockY(int x, int z) const {
    for (int y = 128 - 1; y >= 0; y--) {
        if (blocks[x][y][z] != BlockType::Air) {
            return y;
        }
    }
    return -1;
}
