#pragma once

#include <engine_core/utils/types.hpp>

class MaterialFactory {
public:
    enum class Type {
        Gold,
        Silver,
        Emerald,
        Plastic,
        Rubber
    };

    static Material create(Type type) {
        switch (type) {
        case Type::Gold: return s_gold;
        case Type::Silver: return s_silver;
        case Type::Emerald: return s_emerald;
        case Type::Plastic: return s_plastic;
        case Type::Rubber: return s_rubber;
        }
        return { {0.2f,0.2f,0.2f}, {0.8f,0.8f,0.8f}, {1.0f,1.0f,1.0f}, 32.0f };
    }

private:
    static inline const Material s_gold = { {0.24725f,0.1995f,0.0745f}, {0.75164f,0.60648f,0.22648f}, {0.628281f,0.555802f,0.366065f}, 51.2f };
    static inline const Material s_silver = { {0.19225f,0.19225f,0.19225f}, {0.50754f,0.50754f,0.50754f}, {0.508273f,0.508273f,0.508273f}, 51.2f };
    static inline const Material s_emerald = { {0.0215f,0.1745f,0.0215f}, {0.07568f,0.61424f,0.07568f}, {0.633f,0.727811f,0.633f}, 76.8f };
    static inline const Material s_plastic = { {0.0f,0.0f,0.0f}, {0.55f,0.55f,0.55f}, {0.70f,0.70f,0.70f}, 32.0f };
    static inline const Material s_rubber = { {0.02f,0.02f,0.02f}, {0.01f,0.01f,0.01f}, {0.4f,0.4f,0.4f}, 10.0f };
};

