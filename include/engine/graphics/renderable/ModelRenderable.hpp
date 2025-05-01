#pragma once
#include "IRenderable.hpp"
#include <engine/graphics/model.hpp>

class ModelRenderable : IRenderable {
public:
    ModelRenderable(const Model& m) : model(m) {}

    void draw(const Shader& shader) const override {
        model.draw(shader);
    }
private:
    const Model& model;
};
