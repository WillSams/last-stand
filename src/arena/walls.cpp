#include "walls.h"

// Walls live in a solid grid: Arena::IsSolid answers "is this cell solid?",
// and this resolver is the only thing that drags a body back out of one. A
// body is always moved THEN resolved, one axis at a time — two-axis motion in
// one step would need a normal, which is exactly what a grid snap avoids.

namespace last_stand {

void ResolveAgainstWalls(glm::vec2 &position, glm::vec2 &velocity,
                         int width, int height, int tileSize,
                         bool (*isSolid)(int col, int row, void *ctx),
                         void *ctx) {
    // Horizontal: done first so diagonal pushes don't see a moving target.
    {
        const int left = static_cast<int>(position.x) / tileSize;
        const int right = static_cast<int>(position.x + width - 1) / tileSize;
        const int top = static_cast<int>(position.y) / tileSize;
        const int bot = static_cast<int>(position.y + height - 1) / tileSize;

        if (velocity.x < 0.0f) {
            // Moving left: catch on the left edge of the body.
            if (isSolid(left, top, ctx) || isSolid(left, bot, ctx)) {
                position.x = static_cast<float>((left + 1) * tileSize);
                velocity.x = 0.0f;
            }
        } else if (velocity.x > 0.0f) {
            if (isSolid(right, top, ctx) || isSolid(right, bot, ctx)) {
                position.x = static_cast<float>(right * tileSize - width);
                velocity.x = 0.0f;
            }
        }
    }

    {
        const int left = static_cast<int>(position.x) / tileSize;
        const int right = static_cast<int>(position.x + width - 1) / tileSize;
        const int top = static_cast<int>(position.y) / tileSize;
        const int bot = static_cast<int>(position.y + height - 1) / tileSize;

        if (velocity.y < 0.0f) {
            if (isSolid(left, top, ctx) || isSolid(right, top, ctx)) {
                position.y = static_cast<float>((top + 1) * tileSize);
                velocity.y = 0.0f;
            }
        } else if (velocity.y > 0.0f) {
            if (isSolid(left, bot, ctx) || isSolid(right, bot, ctx)) {
                position.y = static_cast<float>(bot * tileSize - height);
                velocity.y = 0.0f;
            }
        }
    }
}

} // namespace last_stand