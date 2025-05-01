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

#include <engine/graphics/shader.hpp>
#include <engine/graphics/texture.hpp>
#include <engine/utils/types.hpp>

struct Mesh {
    GLuint VBO, VAO, EBO;
    GLsizei indexCount;
    std::shared_ptr<Texture> texture;
    std::vector<std::pair<TextureType, std::shared_ptr<Texture>>> textures;

    //std::vector<std::pair<TextureType ,Texture>> textures;

    void draw(const Shader& shader) const;
};


class Model {
public:
    static void SetBasePath(const std::string& basePath);
    static const std::string& GetBasePath();

    void draw(const Shader& shader) const;

    Model(const std::string& modelPath);

private:
    void loadModel(const std::string& path);
    Mesh processMesh(aiMesh* aMesh, aiMaterial* aMaterial);
    std::shared_ptr<Texture> loadTextures(aiMaterial* material, aiTextureType type) const;
    static std::string s_basePath;

    std::string path;
    std::vector<Mesh> meshes;
};