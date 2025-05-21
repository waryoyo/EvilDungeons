#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <engine_core/graphics/shader.hpp>

class ShaderManager {
public:
    // no instances please
    ShaderManager() = delete;
    ~ShaderManager() = delete;
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    static void Load(const std::string& name,
        const std::string& vertexPath,
        const std::string& fragmentPath);

    static Shader* Get(const std::string& name);
    static void Clear();


private:
    static std::unordered_map<std::string, std::unique_ptr<Shader>> s_shaders;
};

