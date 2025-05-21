#pragma once

#include <unordered_map>
#include <memory>
#include <engine_core/graphics/texture.hpp>
#include <string>

class TextureManager {
public:
private:
	TextureManager() : m_basePath("assets/textures/") {}

	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

    Texture* load(const std::string& filename) {
        auto it = m_textures.find(filename);
        if (it != m_textures.end()) {
            return it->second.get();
        }
        std::string fullPath = m_basePath + filename;
        auto tex = std::make_unique<Texture>(fullPath);
        m_textures.emplace(filename, tex);
        return tex.get();
    }

	std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
	std::string m_basePath;

};
