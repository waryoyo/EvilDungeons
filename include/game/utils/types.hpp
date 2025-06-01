#pragma once

#include <glm/glm.hpp>
#include <iostream>

enum class BlockType : uint8_t {
    Air,
    Dirt,
    Grass,
    Stone,
    Water,
    Sand
};

struct Quad {
    uint32_t x, y, w, h;
};

// ✅ Marked inline to avoid multiple definition errors
inline std::ostream& operator<<(std::ostream& os, BlockType type) {
    switch (type) {
        case BlockType::Air:    return os << "Air";
        case BlockType::Dirt:   return os << "Dirt";
        case BlockType::Grass:  return os << "Grass";
        case BlockType::Stone:  return os << "Stone";
        case BlockType::Water:  return os << "Water";
        case BlockType::Sand:   return os << "Sand";
        default:                return os << "Unknown";
    }
}
