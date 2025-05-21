#pragma once

#include <engine_core/graphics/shader.hpp>

class IRenderable {
public:
	virtual ~IRenderable() = default;
	virtual void draw(const Shader* shader) const = 0;
};
