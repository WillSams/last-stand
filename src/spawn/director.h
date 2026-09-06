#pragma once
#include <cmath>

// Spawn ramp: continuous rising rate, never zero at t=0.
// base + gain * (1 - exp(-t/tau)). Monotonic, concave, no plateau.
namespace last_stand {
inline float SpawnsPerSecond(float t) {
    if (t <= 0.0f) return 0.5f;
    return 0.5f + 3.0f * (1.0f - std::exp(-t / 90.0f));
}
// Spawn point outside camera but inside world. Plain floats, no SDL.
struct SpawnPoint { float x, y; };
inline SpawnPoint SpawnPointOutside(float camX, float camY, float camW, float camH,
                                    float worldW, float worldH, float margin,
                                    unsigned &seed) {
    seed = seed * 1664525u + 1013904223u;
    int edge = static_cast<int>((seed >> 16) % 4u);
    float x = 0, y = 0;
    seed = seed * 1664525u + 1013904223u;
    float frac = static_cast<float>(seed % 1000u) / 1000.0f;
    switch (edge) {
        case 0: x = camX - margin; y = camY + frac * camH; break;
        case 1: x = camX + camW + margin; y = camY + frac * camH; break;
        case 2: y = camY - margin; x = camX + frac * camW; break;
        default: y = camY + camH + margin; x = camX + frac * camW; break;
    }
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > worldW - 1) x = worldW - 1;
    if (y > worldH - 1) y = worldH - 1;
    return {x, y};
}
}  // namespace last_stand
