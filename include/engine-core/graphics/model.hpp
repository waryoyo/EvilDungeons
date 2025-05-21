#pragma once

#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <utility>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <engine_core/graphics/shader.hpp>
#include <engine_core/graphics/texture.hpp>
#include <engine_core/utils/types.hpp>
#include <engine_core/graphics/mesh.hpp>

class Model {
public:
    static void SetBasePath(const std::string& basePath);
    static const std::string& GetBasePath();

    void draw(const Shader* shader) const;

    Model(const std::string& modelPath);

private:
    void loadModel(const std::string& path);
    Mesh processMesh(aiMesh* aMesh, aiMaterial* aMaterial);
    std::shared_ptr<Texture> loadTextures(aiMaterial* material, aiTextureType type) const;
    static std::string s_basePath;

    std::string path;
    std::vector<Mesh> meshes;
};
