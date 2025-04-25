#pragma once

#include <string>
#include <iostream>

#include <glad/glad.h>
#include <stb_image.h>


class Texture {
public:
	static void SetBasePath(const std::string& basePath);
	static const std::string& GetBasePath();

	Texture(const std::string& texturePath,
		GLenum wrapS = GL_REPEAT,
		GLenum wrapT = GL_REPEAT,
		GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR,
		GLenum magFilter = GL_LINEAR
	);

	~Texture();

	void bind(GLuint unit) const;
	void unbind() const;


private:
	GLuint id = 0;

	int m_width = 0;
	int m_height = 0;
	int m_channels = 0;

	static std::string s_basePath;

	std::string loadFile(const std::string& path);
};