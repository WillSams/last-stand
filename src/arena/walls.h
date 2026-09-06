#pragma once

#include <glm/glm.hpp>

// Grid-snapped wall resolution, one axis at a time. Never computes a normal —
// the pattern the tilemap-collision rule demands, because per-tile collider
// boxes catch on seams and push bodies out sideways along a wall.
//
// `isSolid(col, row)` is the only arena interface the resolver asks about;
// playState closes over Arena::IsSolid. Remains free of SDL and of storm, so
// a spec can pin the resolution without either.

namespace last_stand {

// Resolves a body of pixel size (width, height) moving against the solid
// grid. Mutates `position` and `velocity` in place.
//
// The caller moves movement first, then the resolver: that's the standard
// swept-vs-grid pattern, and it stays cheap because the box corners are the
// only cells that matter.
void ResolveAgainstWalls(glm::vec2 &position, glm::vec2 &velocity,
                         int width, int height, int tileSize,
                         bool (*isSolid)(int col, int row, void *ctx),
                         void *ctx);

} // namespace last_stand