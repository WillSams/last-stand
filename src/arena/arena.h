#pragma once

#include <SDL2/SDL.h>

#include <stormengine2/assetStore.h>
#include <stormengine2/tilemapLoader.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>

// The arena: a tilemap plus a solidity grid. Walls are NOT collider entities —
// the game resolves the player against IsSolid() one axis at a time, which
// never computes a normal and so cannot catch on tile seams. See the design
// doc's "Walls are not entities".
class Arena {
public:
    Arena(const std::string &mapPath, int tileSize);

    // True when the tile at (col, row) is a wall. Out-of-range is solid, so a
    // body can never wander past the map edge.
    bool IsSolid(int col, int row) const;

    // World size in pixels, cols * tileSize by rows * tileSize.
    glm::vec2 PixelSize() const;

    // Blits every tile the camera can see, with the camera offset subtracted.
    // The tilesheet is one texture ("tiles") and srcX/srcY per tile come from
    // the map's own lines.
    void Draw(SDL_Renderer *renderer, const storm::AssetStore &assets,
              const SDL_Rect &camera) const;

    int TileSize() const { return tileSize_; }

private:
    int tileSize_;
    int cols_ = 0;
    int rows_ = 0;
    std::vector<storm::Tile> tiles_;
    std::vector<bool> solid_;
};