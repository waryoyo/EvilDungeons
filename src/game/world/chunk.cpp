#include <game/world/chunk.hpp>

#include <engine/graphics/renderable/IRenderable.hpp>
#include <engine/graphics/renderable/meshRenderable.hpp>

#include <engine/graphics/meshFactory.hpp>
#include <engine/graphics/managers/shaderManager.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <iostream>


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

bool Chunk::isAir(glm::ivec3 pos) const {
    if (pos.x < 0 || pos.x >= SIZE || pos.y < 0 || pos.y >= SIZE || pos.z < 0 || pos.z >= SIZE)
        return true;
    return blocks[pos.x][pos.y][pos.z] == BlockType::Air;
}

Chunk::Chunk(glm::ivec3 position) : position(position)
{
    if (!ShaderManager::Get("worldShader"))
        ShaderManager::Load("worldShader", "light/light.vert", "light/light.frag");

 /*   GLuint VAO, VBO, EBO;
    MeshFactory::makeCube(VAO, VBO, EBO);*/
    emissiveBinder = std::make_unique<EmissiveBinder>(glm::vec3(0.1f, 0.8f, 0.05f));
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

void Chunk::buildMesh()
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
        
    uint32_t indexOffset = 0;

    for (int x = 0; x < SIZE; x++) {
        for (int z = 0; z < SIZE; z++) {
            for (int y = 0; y < SIZE; y++) {
                auto blockPos = glm::vec3(x, y, z);
                for (int i = 0; i < 6; i++) {
                    auto normal = faceNormals[i];
                    if (!isAir({ x + normal.x, y + normal.y, z + normal.z }))
                        continue;

                    for (int j = 0; j < 4; j++) {
                        vertices.push_back({
                            blockPos + faceVertices[i][j],
                            baseUVs[j],
                            glm::vec3(normal)
                        });
                    }

                    indices.push_back(indexOffset + 0);
                    indices.push_back(indexOffset + 1);
                    indices.push_back(indexOffset + 2);
                    indices.push_back(indexOffset + 2);
                    indices.push_back(indexOffset + 3);
                    indices.push_back(indexOffset + 0);
                    indexOffset += 4;
                }
            }
        }
    }


    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);

    glNamedBufferData(VBO, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
    glNamedBufferData(EBO, sizeof(indices), indices.data(), GL_STATIC_DRAW);

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
