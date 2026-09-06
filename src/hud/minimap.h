#pragma once
#include <glm/glm.hpp>

// World -> minimap transform, pure. No SDL.
namespace last_stand {
struct MiniPanel { float x, y, w, h; };
inline glm::vec2 WorldToMinimap(glm::vec2 world, glm::vec2 worldSize, MiniPanel p) {
    float sx = world.x / (worldSize.x > 0 ? worldSize.x : 1.0f);
    float sy = world.y / (worldSize.y > 0 ? worldSize.y : 1.0f);
    if (sx < 0) sx = 0; if (sx > 1) sx = 1;
    if (sy < 0) sy = 0; if (sy > 1) sy = 1;
    return glm::vec2(p.x + sx * p.w, p.y + sy * p.h);
}
}  // namespace last_stand
