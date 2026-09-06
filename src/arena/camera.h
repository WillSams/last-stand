#pragma once

#include <algorithm>
#include <glm/glm.hpp>

// Centres a view of (viewW, viewH) pixels on `target`, then clamps so it never
// shows outside the world. When the world is smaller than the view on an axis,
// the view pins to 0 rather than going negative.
//
// Returns the top-left corner; the caller builds its SDL_Rect from it. Keeping
// SDL out of this header is deliberate — the math is what is worth testing,
// and an SDL_RECT is just an int quad a spec can assemble itself.
inline glm::ivec2 CameraTopLeft(glm::vec2 target, int viewW, int viewH,
                                glm::vec2 worldSize) {
    const float maxX = std::max(0.0f, worldSize.x - static_cast<float>(viewW));
    const float maxY = std::max(0.0f, worldSize.y - static_cast<float>(viewH));
    const float x = std::min(std::max(target.x - viewW * 0.5f, 0.0f), maxX);
    const float y = std::min(std::max(target.y - viewH * 0.5f, 0.0f), maxY);
    return glm::ivec2(static_cast<int>(x), static_cast<int>(y));
}