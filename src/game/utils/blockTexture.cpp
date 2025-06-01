#include <game/utils/blockTexture.hpp>

glm::vec4 getBlockFaceUV(BlockType block, BlockFace face) {
    auto it = blockTileMap.find(block);
    if (it == blockTileMap.end()) {
        // fallback UV or error case
        return glm::vec4(0, 0, 1, 1);
    }
    Tile tile;
    switch(face) {
        case BlockFace::Top: tile = it->second.top; break;
        case BlockFace::Bottom: tile = it->second.bottom; break;
        case BlockFace::Side: tile = it->second.side; break;
    }

    constexpr float atlasSize = 256.0f; // pixels
    float uMin = (tile.x * tilePx) / atlasSize;
    float vMin = (tile.y * tilePx) / atlasSize;
    float uMax = ((tile.x + 1) * tilePx) / atlasSize;
    float vMax = ((tile.y + 1) * tilePx) / atlasSize;

    // OpenGL might flip v coords, adjust if needed
    return glm::vec4(uMin, vMax, uMax, vMin);
}
