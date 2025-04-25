#include <engine/graphics/texture.hpp>

std::string Texture::s_basePath = "assets/textures/";

Texture::Texture(const std::string& texturePath, GLenum wrapS, GLenum wrapT, GLenum minFilter, GLenum magFilter)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, wrapS, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, wrapT, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, minFilter, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, magFilter, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    int width, height, colorChannels;

    const std::string fullPath = s_basePath + texturePath;

    unsigned char* imageData =
        stbi_load(fullPath.c_str(), &width, &height, &colorChannels, 0);

    if (!imageData) {
        std::cerr << "GRRR";
        exit(0);
    }

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGB,
        width, height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        imageData);

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(imageData);
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

void Texture::bind(GLuint unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
