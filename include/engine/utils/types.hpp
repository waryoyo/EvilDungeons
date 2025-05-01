#pragma once

#include <glm/glm.hpp>

enum class TextureType {
    Diffuse,
    Specular,
    Normal,
    Height
};

struct MVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    glm::vec3 tangent;
    glm::vec3 bitTangent;
    int boneIDs[4];
    float weights[4];
};

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shiniess;
};