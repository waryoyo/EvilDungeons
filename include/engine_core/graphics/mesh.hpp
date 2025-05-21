#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

#include <engine_core/utils/types.hpp>
#include <engine_core/graphics/texture.hpp>
#include <engine_core/graphics/shader.hpp>


struct Mesh {
    GLuint VBO, VAO, EBO;
    GLsizei indexCount;
    std::vector<std::pair<TextureType, std::shared_ptr<Texture>>> textures;

    //std::vector<std::pair<TextureType ,Texture>> textures;

    void draw(const Shader* shader) const;
};

