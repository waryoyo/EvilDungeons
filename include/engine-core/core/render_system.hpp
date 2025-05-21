#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <engine_core/graphics/meshFactory.hpp>

#include <glm/glm.hpp>

#include <engine_core/graphics/renderable/MeshRenderable.hpp>
#include <engine_core/graphics/renderable/ModelRenderable.hpp>
#include <engine_core/components/rendererComponent.hpp>
#include <engine_core/graphics/binder/emissiveBinder.hpp>
#include <engine_core/graphics/meshFactory.hpp>
#include <engine_core/graphics/binder/phongBinder.hpp>
#include <engine_core/graphics/binder/IUniformBinder.hpp>

#include <engine_core/graphics/factories/materialFactory.hpp>

#include <engine_core/core/gameObject.hpp>


#include <vector>

class RenderSystem {
public:

    RenderSystem() {
        phongBinder = std::make_unique<PhongBinder>(MaterialFactory::create(MaterialFactory::Type::Silver));
        emissiveBinder = std::make_unique<EmissiveBinder>(glm::vec3(1.0f, 1.0f, 1.0f));
    }

    void renderAll(const RenderContext& ctx) {
      
    }

  /*  RendererComponent* createTexturedCube(GameObject* owner,
        const std::string& texturePath,
        Shader* shader)
    {
        GLuint VAO, VBO, EBO;
        MeshFactory::makeCube(VAO, VBO, EBO); 

        auto mesh = std::make_unique<MeshRenderable>(Texture(texturePath));
        auto rc = std::make_unique<RendererComponent>(owner, std::move(mesh), shader);
        owner->addComponent(rc.get());
        renderComps.push_back(std::move(rc));
        return renderComps.back().get();
    }
    */
    void addColorCube(GameObject* owner,
        const glm::vec3& color,
        Shader* shader)
    {
        GLuint VAO, VBO, EBO;
        MeshFactory::makeCube(VAO, VBO, EBO);
        auto meshRenderer = std::make_unique<MeshRenderable>(VAO, 36);
        auto rc = std::make_unique<RendererComponent>(owner, std::move(meshRenderer), shader, MaterialFactory::create(MaterialFactory::Type::Silver), emissiveBinder.get(), false);
        owner->addComponent(std::move(rc));
    }

    void addModel(GameObject* owner,
        const std::string& modelPath,
        Shader* shader)
    {
        auto mesh = std::make_unique<ModelRenderable>(std::move(std::make_unique<Model>(modelPath)));
        auto rc = std::make_unique<RendererComponent>(owner, std::move(mesh), shader, MaterialFactory::create(MaterialFactory::Type::Silver), phongBinder.get(), true);
        owner->addComponent(std::move(rc));
    }

private:
    std::unique_ptr<PhongBinder> phongBinder;
    std::unique_ptr<EmissiveBinder> emissiveBinder;
};

