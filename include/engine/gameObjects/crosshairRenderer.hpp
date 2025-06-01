#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <engine/graphics/managers/shaderManager.hpp>
#include <stb_image.h>
#include <iostream>
#include <string>

class Shader;

class CrosshairRenderer {
public:
    CrosshairRenderer(const std::string& texturePath, Shader* shader);
    ~CrosshairRenderer();

    void render(int screenWidth, int screenHeight);

private:
    GLuint VAO, VBO, EBO, textureID;
    Shader* shader;

    void loadTexture(const std::string& path);
    void setupQuad();
};
