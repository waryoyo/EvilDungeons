#include <engine_core/graphics/binder/phongBinder.hpp>

void PhongBinder::apply(const BinderParams& params) const
{
    const auto shader = params.shader;
    const auto& lights = params.context.lights;

    const auto mvp = params.context.cameraData.VP * params.modelMatrix;
    const auto numLights = std::min(static_cast<int>(lights.size()), 16);

    shader->setMat4("uMVP", mvp);
    shader->setMat4("uModel", params.modelMatrix);
    shader->setVec3("cameraPos", params.context.cameraData.cameraPos);
    shader->setFloat("shininess", material.shiniess);
    shader->setInt("numLights", numLights);

    for (int i = 0; i < numLights; i++) {
        shader->setVec3(std::string("lights[") + std::to_string(i) + "].ambient", lights[i].ambient);
        shader->setVec3(std::string("lights[") + std::to_string(i) + "].diffuse", lights[i].diffuse);
        shader->setVec3(std::string("lights[") + std::to_string(i) + "].specular", lights[i].specular);
        shader->setVec3(std::string("lights[") + std::to_string(i) + "].lightColor", lights[i].lightColor);
        shader->setVec3(std::string("lights[") + std::to_string(i) + "].lightPos", lights[i].lightPos);
        shader->setFloat(std::string("lights[") + std::to_string(i) + "].range", lights[i].range);
    }

}

