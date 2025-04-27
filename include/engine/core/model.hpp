#pragma once

#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <utility>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <engine/graphics/texture.hpp>

enum class TextureType {
    Diffuse,
    Specular,
    Normal,
    Height
};

struct MVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    glm::vec3 tangent;
    glm::vec3 bitTangent;
    int boneIDs[4];
    float weights[4];
};

struct Mesh {
    GLuint VBO, VAO, EBO;
    GLsizei indexCount;
    std::vector<std::pair<TextureType ,Texture>> textures;

    void draw() const;
};


class Model {
public:
    static void SetBasePath(const std::string& basePath);
    static const std::string& GetBasePath();

    void draw() const;

    Model(const std::string& modelPath);

private:
    void loadModel(const std::string& path);
    Mesh processMesh(aiMesh* aMesh, aiMaterial* aMaterial);
    std::vector<Texture> loadTextures(aiMaterial* material, aiTextureType type) const;
    static std::string s_basePath;

    std::string path;
    std::vector<Mesh> meshes;
};