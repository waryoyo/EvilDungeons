#pragma once

#include <glad/glad.h>
#include <memory>
#include <vector>

struct Mesh {
    GLuint VBO, VAO, EBO;
    GLsizei indexCount;
    std::shared_ptr<Texture> texture;
    std::vector<std::pair<TextureType, std::shared_ptr<Texture>>> textures;

    //std::vector<std::pair<TextureType ,Texture>> textures;

    void draw(const Shader& shader) const;
};
