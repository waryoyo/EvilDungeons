#include <engine/gameObjects/crosshairRenderer.hpp>


CrosshairRenderer::CrosshairRenderer(const std::string& texturePath, Shader* shader)
    : shader(shader) {
    if (!shader) {
        std::cerr << "Crosshair shader not found!" << std::endl;
        return;
    }
    loadTexture(texturePath);
    setupQuad();
}

CrosshairRenderer::~CrosshairRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &textureID);
}

void CrosshairRenderer::loadTexture(const std::string& path) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (data) {
        std::cout << "Loaded crosshair texture: " << path << " (" << width << "x" << height << ")" << std::endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load crosshair texture: " << path << std::endl;
    }
    stbi_image_free(data);
}

void CrosshairRenderer::setupQuad() {
    float halfSize = 0.15f; // NDC scale, ~5% of screen height/width
    float quadVertices[] = {
        // positions       // texCoords
        -halfSize, -halfSize,   0.0f, 1.0f,
         halfSize, -halfSize,   1.0f, 1.0f,
         halfSize,  halfSize,   1.0f, 0.0f,
        -halfSize,  halfSize,   0.0f, 0.0f
    };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}


void CrosshairRenderer::render(int screenWidth, int screenHeight) {
    shader->use();

    // Pass screen size to shader
    shader->setVec2("screenSize", glm::vec2(screenWidth, screenHeight));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(shader->getID(), "crosshairTexture"), 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


