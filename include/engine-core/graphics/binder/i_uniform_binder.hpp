#pragma once

#include <engine_core/graphics/shader.hpp>

#include <vector>
#include <engine_core/utils/types.hpp>

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
