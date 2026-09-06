#pragma once
#include <cmath>
#include <glm/glm.hpp>

// Degrees, clockwise from +X, because SDL_RenderCopyEx takes degrees and the
// Kenney characters are drawn facing right. A constant, not M_PI: M_PI is not
// in the C++ standard and needs _USE_MATH_DEFINES to exist on the MinGW build
// that Makefile.win targets.
constexpr float kPi = 3.14159265f;

inline float AimAngleDegrees(glm::vec2 from, glm::vec2 to) {
    const glm::vec2 d = to - from;
    if (d.x == 0.0f && d.y == 0.0f) return 0.0f;
    return static_cast<float>(std::atan2(d.y, d.x) * 180.0 / kPi);
}