#pragma once

#include <cmath>

#include <glm/glm.hpp>
#include <stormengine2/ecs.h>

#include "../components/gameComponents.h"
#include "../input/aim.h"

namespace last_stand {

// Steers every enemy toward the player. Pure-ish: reads components in place,
// writes only velocity and rotation. Kept out of a System subclass so a spec
// can drive it with a hand-seeded Registry and no frame loop.
//
// This is the single most important gameplay rule: enemies always seek the
// player, and face where they are moving. The speed comes from BrainComponent
// so a spec can slow one and watch the other converge.
struct EnemySteer {
    // Runs the steer against every BrainComponent + TransformComponent pair on
    // the registry. Entity lifetime is respected: Killed enemies drop out of
    // the loop via RemoveEntityFromSystems on the next registry.Update().
    static void Steer(storm::Registry &registry, glm::vec2 playerPos);
};

} // namespace last_stand