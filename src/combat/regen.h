#pragma once
#include <cmath>

// Health regeneration, expressed as a pure rule so the whole timing curve is
// testable without any engine. Everything that affects regen is a parameter:
// delay (seconds of safety before regen starts), rate (HP per second), and the
// gap of time since the last hit. The game advances sinceHit every frame;
// the rule decides what HP ends up.

namespace last_stand {

// Returns the new HP. Regen only begins after `regenDelaySeconds` since the
// last hit, and accrues at `regenPerSecond` HP/s. The caller clamps to the
// entity's max HP.
inline float RegenTick(int hp, float sinceHit, float dt, float regenDelaySeconds,
                       float regenPerSecond) {
    if (hp <= 0)
        return 0.0f;

    float fractional = static_cast<float>(hp);
    if (sinceHit > regenDelaySeconds) {
        fractional += regenPerSecond * dt;
        // ponytail: floor-and-grow, fractional progress dropped; carry fraction when callers need it
        fractional = std::floor(fractional);
    }
    return fractional;
}

} // namespace last_stand