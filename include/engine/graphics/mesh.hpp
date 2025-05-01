#pragma once

#include <glad/glad.h>
#include <memory>
#include <vector>

#include <engine/utils/types.hpp>
#include <engine/graphics/texture.hpp>
#include <engine/graphics/shader.hpp>


struct Mesh {
    GLuint VBO, VAO, EBO;
    GLsizei indexCount;
    std::vector<std::pair<TextureType, std::shared_ptr<Texture>>> textures;

    //std::vector<std::pair<TextureType ,Texture>> textures;

    void draw(const Shader& shader) const;
};
