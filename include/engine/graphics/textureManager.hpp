#pragma once

#include <unordered_map>
#include <memory>
#include <engine/graphics/texture.hpp>
#include <string>



class TextureManager {
public:
private:
	TextureManager() : m_basePath("assets/textures/") {}

	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

	std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
	std::string m_basePath;

};