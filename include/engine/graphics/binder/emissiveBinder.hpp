#pragma once

#include <engine/graphics/binder/IUniformBinder.hpp>
#include <engine/graphics/texture.hpp>
#include <engine/utils/types.hpp>
#include <variant>
#include <vector>

class EmissiveBinder : public IUniformBinder {
public:
    EmissiveBinder(const glm::vec3& color)
        : emissive(color) {}
    EmissiveBinder(const Texture* texture)
        : emissive(texture) {}
    void apply(const BinderParams& params) const override;
private:
    std::variant<glm::vec3, const Texture*> emissive;
};