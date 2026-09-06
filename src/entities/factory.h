#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <stormengine2/ecs.h>

#include "../combat/pickups.h"

namespace last_stand {

enum class EnemyKind { Zombie, Robot, Old };

// What an enemy IS, from a single table. Sprite sizes are the images' own
// dimensions (this pack is not uniform); collider radius is half the sprite
// on its longer axis.
struct EnemySpec {
    const char *textureId;  // asset store key — must be loaded already
    int width = 0;          // sprite source rect = the PNG's own dimensions
    int height = 0;
    float radius = 0.0f;    // circle collider radius
    int hp = 3;             // hits to kill
    float speed = 60.0f;    // chase speed, world px/s
    int damage = 1;         // contact damage to the player
};

constexpr EnemySpec EnemySpecFor(EnemyKind kind) {
    switch (kind) {
    case EnemyKind::Zombie:
        // zoimbie1_hold.png is 35x43 — the upstream typo in the filename is
        // intentional; see CLAUDE.md.
        return EnemySpec{"zombie", 35, 43, 17.0f, 2, 45.0f, 1};
    case EnemyKind::Robot:
        return EnemySpec{"robot", 35, 43, 17.0f, 4, 75.0f, 1};
    case EnemyKind::Old:
        return EnemySpec{"old", 35, 43, 17.0f, 1, 35.0f, 2};
    }
    return EnemySpec{"", 0, 0, 0.0f, 0, 0.0f, 0};
}

// Spawns an enemy of the given kind at `at`, with the full component set the
// engine must see at admission. `encounterScale` lets the spawner widen
// health/damage as the run progresses.
storm::Entity SpawnEnemy(storm::Registry &registry, glm::vec2 at,
                         EnemyKind kind, float speedScale = 1.0f);

struct PickupComponent : storm::Component<PickupComponent> {
    PickupKind kind = PickupKind::Gun;
    explicit PickupComponent(PickupKind k = PickupKind::Gun) : kind(k) {}
};

storm::Entity SpawnPickup(storm::Registry &registry, glm::vec2 at, PickupKind kind);

} // namespace last_stand