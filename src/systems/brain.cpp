#include "brain.h"

#include <stormengine2/components/rigidBody.h>
#include <stormengine2/components/transform.h>

#include "../components/gameComponents.h"
#include "../input/aim.h"

namespace last_stand {

void EnemySteer::Steer(storm::Registry &registry, glm::vec2 playerPos) {
    for (const storm::Entity &entity :
         registry.GetEntitiesByGroup("enemies")) {
        auto &transform = entity.GetComponent<storm::TransformComponent>();
        auto &rb = entity.GetComponent<storm::RigidBodyComponent>();
        auto &brain = entity.GetComponent<BrainComponent>();

        const glm::vec2 toPlayer = playerPos - transform.position;
        const float dist = std::sqrt(toPlayer.x * toPlayer.x +
                                     toPlayer.y * toPlayer.y);

        if (dist < 1.0f) {
            // Right on top of the player: stop, don't jitter.
            rb.velocity = glm::vec2(0.0f, 0.0f);
            continue;
        }

        rb.velocity = toPlayer / dist * brain.speed;
        transform.rotation = AimAngleDegrees(transform.position, playerPos);
    }
}

} // namespace last_stand