#pragma once

#include <glm/glm.hpp>
#include <vector>

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

    //glm::vec3 tangent;
    //glm::vec3 bitTangent;
    //int boneIDs[4];
    //float weights[4];
};

struct Light {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct LightNew {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 lightPos;
    glm::vec3 lightColor;
    float range;
};

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shiniess;
};

struct CameraData {
    glm::vec3 cameraPos;
    glm::mat4 VP;
};


struct RenderContext {
    CameraData cameraData;
    const std::vector<LightNew>& lights;
    float globalShininess = 32.0f;
};

