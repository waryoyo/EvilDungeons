#include <engine/graphics/binder/emissiveBinder.hpp>

void EmissiveBinder::apply(const BinderParams& params) const
{
    const auto mvp = params.context.cameraData.VP * params.modelMatrix;
    const auto shader = params.shader;

    shader->use();
    shader->setMat4("uMVP", mvp);
    shader->setMat4("uModel", params.modelMatrix);

    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, const Texture*>) {
            arg->bind(0);
            shader->setInt("uTex", 0);
        }
        else if constexpr (std::is_same_v<T, glm::vec4>) {
            shader->setVec3("uObjectColor", arg);
        }
    }, emissive);

}
