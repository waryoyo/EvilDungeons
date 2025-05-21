#pragma once

#include <glm/glm.hpp>

enum class BlockType : uint8_t {
    Air,
    Dirt,
    Grass,
    Stone,
    Water
};

struct Quad {
    uint32_t x, y, w, h;
};
