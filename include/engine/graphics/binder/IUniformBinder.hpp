#pragma once

#include <engine/graphics/shader.hpp>

#include <vector>
#include <engine/utils/types.hpp>

struct BinderParams {
    Shader* shader;
    glm::mat4 modelMatrix;
    const RenderContext& context;
};

class IUniformBinder {
public:
	virtual ~IUniformBinder() = default;
	virtual void apply(const BinderParams& params) const = 0;
};