#pragma once
#include <glm/glm.hpp>

// Keyboard + gamepad move input, already sampled, no SDL types so specs link
// with no renderer. Mirrors nightshift's input.h: sticks and d-pad stay
// separate because Gamepad::Down() folds stick pushes into d-pad flags.
namespace last_stand {
struct MoveInput {
    bool left = false, right = false, up = false, down = false;
    float stickX = 0.0f, stickY = 0.0f;
    bool dpadLeft = false, dpadRight = false, dpadUp = false, dpadDown = false;
};

// All sources into one velocity of magnitude <= speed. D-pad honoured only
// while stick reads dead, else stick+d-pad would double speed.
inline glm::vec2 PlayerSteering(const MoveInput &in, float speed) {
    glm::vec2 v(0.0f);
    if (in.left) v.x -= 1.0f;
    if (in.right) v.x += 1.0f;
    if (in.up) v.y -= 1.0f;
    if (in.down) v.y += 1.0f;
    v.x += in.stickX;
    v.y += in.stickY;
    if (in.stickX == 0.0f && in.stickY == 0.0f) {
        if (in.dpadLeft) v.x -= 1.0f;
        if (in.dpadRight) v.x += 1.0f;
        if (in.dpadUp) v.y -= 1.0f;
        if (in.dpadDown) v.y += 1.0f;
    }
    if (glm::length(v) > 1.0f) v = glm::normalize(v);
    return v * speed;
}
}  // namespace last_stand
