#include <engine/graphics/texture.hpp>

std::string Texture::s_basePath = "assets/textures/";

void Texture::SetBasePath(const std::string& basePath)
{
    s_basePath = basePath;
}

const std::string& Texture::GetBasePath()
{
    return s_basePath;
}

Texture::Texture(const std::string& texturePath, GLenum wrapS, GLenum wrapT, GLenum minFilter, GLenum magFilter)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    const std::string fullPath = s_basePath + texturePath;

    stbi_set_flip_vertically_on_load(1);
    unsigned char* imageData =
        stbi_load(fullPath.c_str(), &m_width, &m_height, &m_channels, 0);

    if (!imageData) {
        std::cerr << "GRRR";
        exit(0);
    }
    GLenum format;
    GLenum internalFormat;

    switch (m_channels) {
    case 1:
        format = GL_RED;
        internalFormat = GL_RED;
        break;
    case 3:
        format = GL_RGB;
        internalFormat = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        internalFormat = GL_RGBA;
        break;
    default:
        format = GL_RGB;
        internalFormat = GL_RGB;
        break;
    }

    glTexImage2D(GL_TEXTURE_2D,
        0,
        internalFormat,
        m_width, m_height,
        0,
        format,
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
