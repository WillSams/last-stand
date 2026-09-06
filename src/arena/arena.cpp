#include "arena.h"

#include <stormengine2/logger.h>

#include <algorithm>

Arena::Arena(const std::string &mapPath, int tileSize) : tileSize_(tileSize) {
    // Editor format (space-separated), so no tileset PNG is needed — srcX/srcY
    // are embedded per tile. Tiles carry assetId "tiles", which LoadAssets
    // registers against the single tilesheet PNG.
    storm::TileMapLoader loader(mapPath, "", tileSize_);

    tiles_ = loader.getMap();
    if (tiles_.empty()) {
        storm::Logger().Err("Arena: map '" + mapPath + "' loaded no tiles.");
        return;
    }

    // Derive the grid size from the tiles: the loader's getMapResolution()
    // reports the tileset PNG's pixel size, not the map's tile count.
    for (const storm::Tile &tile : tiles_) {
        cols_ = std::max(cols_, tile.relativePosition.x + 1);
        rows_ = std::max(rows_, tile.relativePosition.y + 1);
    }

    solid_.assign(static_cast<std::size_t>(cols_) * rows_, false);
    for (const storm::Tile &tile : tiles_) {
        if (!tile.hasCollider)
            continue;
        const int col = tile.relativePosition.x;
        const int row = tile.relativePosition.y;
        if (col < 0 || col >= cols_ || row < 0 || row >= rows_)
            continue;
        solid_[static_cast<std::size_t>(row) * cols_ + col] = true;
    }
}

bool Arena::IsSolid(int col, int row) const {
    if (col < 0 || col >= cols_ || row < 0 || row >= rows_)
        return true;
    return solid_[static_cast<std::size_t>(row) * cols_ + col];
}

glm::vec2 Arena::PixelSize() const {
    return glm::vec2(static_cast<float>(cols_) * tileSize_,
                     static_cast<float>(rows_) * tileSize_);
}

void Arena::Draw(SDL_Renderer *renderer, const storm::AssetStore &assets,
                 const SDL_Rect &camera) const {
    SDL_Texture *texture = assets.GetTexture("tiles");
    if (!texture)
        return;

    for (const storm::Tile &tile : tiles_) {
        const int dstX = tile.relativePosition.x * tileSize_ - camera.x;
        const int dstY = tile.relativePosition.y * tileSize_ - camera.y;

        // Cull tiles entirely outside the view. tileSize_ is the on-screen
        // size because scale is 1 everywhere in the map.
        if (dstX + tileSize_ < 0 || dstX > camera.w)
            continue;
        if (dstY + tileSize_ < 0 || dstY > camera.h)
            continue;

        const SDL_Rect src{tile.pixelSrcPosition.x, tile.pixelSrcPosition.y,
                           tileSize_, tileSize_};
        const SDL_Rect dst{dstX, dstY, tileSize_, tileSize_};
        SDL_RenderCopy(renderer, texture, &src, &dst);
    }
}