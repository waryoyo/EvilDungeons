// BlockTextures.hpp
#pragma once
#include <unordered_map>
#include <game/utils/types.hpp>
#include <glm/glm.hpp>

// Tile size in pixels
constexpr float tilePx = 16.0f;

// Tile position in the atlas
struct Tile { int x, y; };

// Block textures for each face
struct BlockTiles {
    Tile top, bottom, side;
};

// Map block types to their tile coordinates in the atlas
static const std::unordered_map<BlockType, BlockTiles> blockTileMap = {
    { BlockType::Dirt,   { {1,0}, {1,0}, {1,0} } },
    { BlockType::Grass,  { {0,0}, {1,0}, {2,0} } },
    { BlockType::Water,  { {3,0}, {3,0}, {3,0} } },
    { BlockType::Sand,   { {0,11}, {0,11}, {0,11} }},
    { BlockType::Stone,  { {4,0}, {4,0}, {4,0} }}
};

// Enum for block face types
enum class BlockFace {
    Top,
    Bottom,
    Side
};

// UV calculation function declaration
glm::vec4 getBlockFaceUV(BlockType block, BlockFace face);
