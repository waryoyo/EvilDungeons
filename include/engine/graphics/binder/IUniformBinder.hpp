#pragma once

#include <engine/graphics/shader.hpp>

class IUniformBinder {
public:
	virtual ~IUniformBinder() = default;
	virtual void apply(Shader* shader) const = 0;
};