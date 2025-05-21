#include <game_engine/world/chunk.hpp>

#include <engine_core/graphics/renderable/IRenderable.hpp>
#include <engine_core/graphics/renderable/meshRenderable.hpp>

#include <engine_core/graphics/meshFactory.hpp>
#include <engine_core/graphics/factories/materialFactory.hpp>
#include <engine_core/graphics/managers/shaderManager.hpp>
#include <engine_core/graphics/texture.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <bit>


const glm::ivec3 faceNormals[6] = {
    { 1,  0,  0},
    {-1,  0,  0},
    { 0,  1,  0},
    { 0, -1,  0},
    { 0,  0,  1},
    { 0,  0, -1},
};

const glm::vec3 faceVertices[6][4] = {
    {{1,0,0}, {1,1,0}, {1,1,1}, {1,0,1}}, // right
    {{0,0,1}, {0,1,1}, {0,1,0}, {0,0,0}}, // left
    {{0,1,0}, {0,1,1}, {1,1,1}, {1,1,0}}, // top
    {{0,0,1}, {0,0,0}, {1,0,0}, {1,0,1}}, // bottom
    {{0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}}, // front
    {{1,0,0}, {0,0,0}, {0,1,0}, {1,1,0}}, // back
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
        return true;
    return blocks[pos.x][pos.y][pos.z] == BlockType::Air;
}

Chunk::Chunk(glm::ivec3 position) : position(position)
{
    if (!ShaderManager::Get("worldShader"))
        ShaderManager::Load("worldShader", "super_cube/basic.vert", "super_cube/basic.frag");

 /*   GLuint VAO, VBO, EBO;
    MeshFactory::makeCube(VAO, VBO, EBO);*/
    //emissiveBinder = std::make_unique<PhongBinder>(glm::vec3(0.1f, 0.8f, 0.05f));
    emissiveBinder = std::make_unique<EmissiveBinder>(glm::vec3(1.0f, 1.0f, 1.0f));
}

void Chunk::generate() {
    for (int x = 0; x < SIZE; x++) {
        for (int z = 0; z < SIZE; z++) {
            for (int y = 0; y < SIZE; y++) {
                if (y <= 4)
                    blocks[x][y][z] = BlockType::Dirt;
            }
        }
    }
}

std::vector<Quad> Chunk::greedyMesh(std::vector<uint32_t>& data) {
    std::vector<Quad> quads;
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

        indices.insert(indices.end(), {
            indexOffset + 0, indexOffset + 1, indexOffset + 2,
            indexOffset + 0, indexOffset + 2, indexOffset + 3
            });

        indexOffset += 4;
    }
}


void Chunk::buildMesh()
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t indexOffset = 0;

    for (int i = 0; i < 6; i++) { // face
        auto normal = directions[i];
        auto uVec = uAxes[i];
        auto vVec = vAxes[i];
        for (int d = 0; d < SIZE; d++) {
            std::vector<uint32_t> data(SIZE);

            for (int v = 0; v < SIZE; v++) {
                for (int u = 0; u < SIZE; u++) {
                    glm::ivec3 pos = normal * d + uVec * u + vVec * v;
                    glm::ivec3 neighbor = pos + normal;

                    BlockType current = getBlock(pos);
                    BlockType adjacent = getBlock(neighbor);

                    if (current != BlockType::Air && adjacent == BlockType::Air) {
                        data[u] |= (1u << v); 
                    }
                }
            }
            std::vector<Quad> quads = greedyMesh(data);
            glm::ivec3 origin = position + normal * d;
            addVerticesIndices(quads, origin, uVec, vVec, normal, vertices, indices, indexOffset);

        }

    }

    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);

    glNamedBufferData(VBO, sizeof(vertices) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glNamedBufferData(EBO, sizeof(indices) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));

    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
    glVertexArrayAttribBinding(VAO, 0, 0);
    glEnableVertexArrayAttrib(VAO, 0);


    glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
    glVertexArrayAttribBinding(VAO, 1, 0);
    glEnableVertexArrayAttrib(VAO, 1);

    glVertexArrayAttribFormat(VAO, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    glVertexArrayAttribBinding(VAO, 2, 0);
    glEnableVertexArrayAttrib(VAO, 2);

    glVertexArrayElementBuffer(VAO, EBO);
    mesh = std::make_unique<MeshRenderable>(VAO, indices.size());
}

void Chunk::render(const RenderContext& context) {
    if (!ShaderManager::Get("worldShader")) {
        std::cerr << "worldShader not loaded" << '\n';
        return;
    }
    const auto shader = ShaderManager::Get("worldShader");

    const BinderParams params = BinderParams(shader, glm::mat4(1.0f), context);
    shader->use();
    shader->setInt("texture_diffuse", 0);
    emissiveBinder->apply(params);
    mesh->draw(shader);
    /*for (int x = 0; x < SIZE; x++) {
        for (int z = 0; z < SIZE; z++) {
            for (int y = 0; y < SIZE; y++) {
                if (blocks[x][y][z] == BlockType::Dirt) {
                    const auto shader = ShaderManager::Get("worldShader");
                    const BinderParams params = BinderParams(shader, glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)), context);
                    emissiveBinder->apply(params);
                    mesh->draw(shader);
                }
            }
        }
    }*/

}

BlockType Chunk::getBlock(glm::i8vec3 pos) const
{
    if (pos.x < 0 || pos.x >= SIZE || pos.y < 0 || pos.y >= SIZE || pos.z < 0 || pos.z >= SIZE)
        return BlockType::Air;
    return blocks[pos.x][pos.y][pos.z];
}

