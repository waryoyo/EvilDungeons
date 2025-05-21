#include <engine_core/graphics/managers/shaderManager.hpp>

std::unordered_map<std::string, std::unique_ptr<Shader>> ShaderManager::s_shaders;

void ShaderManager::Load(const std::string& name,
    const std::string& vertexPath,
    const std::string& fragmentPath)
{
    if (s_shaders.count(name) != 0)
        return;

    auto shader = std::make_unique<Shader>(vertexPath, fragmentPath);
    s_shaders.emplace(name, std::move(shader));
    return;
}

Shader* ShaderManager::Get(const std::string& name) {
    auto it = s_shaders.find(name);
    if (it == s_shaders.end())
        return nullptr;
    return it->second.get();
}

void ShaderManager::Clear() {
    s_shaders.clear();
}

