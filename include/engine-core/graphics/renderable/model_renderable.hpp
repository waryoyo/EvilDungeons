#pragma once
#include "IRenderable.hpp"
#include <engine_core/graphics/model.hpp>
#include <memory>

class ModelRenderable : public IRenderable {
public:
    ModelRenderable(std::unique_ptr<Model> m) : model(std::move(m)) {}

    void draw(const Shader* shader) const override {
        model->draw(shader);
    }
private:
    std::unique_ptr<Model> model;
};

