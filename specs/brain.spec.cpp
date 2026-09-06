#include <igloo/igloo_alt.h>

#include <cmath>
#include <string>

#include <stormengine2/components/rigidBody.h>
#include <stormengine2/components/transform.h>

#include "../src/entities/factory.h"
#include "../src/systems/brain.h"

using namespace igloo;
using last_stand::EnemyKind;
using last_stand::EnemySpecFor;
using last_stand::EnemySteer;
using last_stand::SpawnEnemy;
namespace ls = last_stand;
using storm::Registry;

namespace {

constexpr float kEps = 1e-3f;

} // namespace

Describe(EnemySteerSpec) {

  It(chases_the_player_along_the_line_to_them) {
    Registry registry;
    ls::SpawnEnemy(registry, {200, 0}, EnemyKind::Zombie);
    ls::EnemySteer::Steer(registry, {0, 0});

    const auto &rb =
        registry.GetEntitiesByGroup("enemies")[0]
            .GetComponent<storm::RigidBodyComponent>();
    // From (200, 0) facing (0, 0), -X is toward the player.
    Assert::That(rb.velocity.x, Is().LessThan(0.0f));
  };

  It(speed_scales_the_velocity) {
    Registry a, b;
    ls::SpawnEnemy(a, {100, 0}, EnemyKind::Zombie);
    ls::SpawnEnemy(b, {100, 0}, EnemyKind::Robot);  // Robot is faster in the table
    ls::EnemySteer::Steer(a, {0, 0});
    ls::EnemySteer::Steer(b, {0, 0});

    const auto &rbA = a.GetEntitiesByGroup("enemies")[0]
        .GetComponent<storm::RigidBodyComponent>();
    const auto &rbB = b.GetEntitiesByGroup("enemies")[0]
        .GetComponent<storm::RigidBodyComponent>();
    // Robot has the higher BrainComponent speed → bigger step towards player.
    Assert::That(std::abs(rbB.velocity.x), Is().GreaterThan(
                                                   std::abs(rbA.velocity.x)));
  };

  It(rotates_the_sprite_to_face_the_player) {
    Registry registry;
    ls::SpawnEnemy(registry, {100, 100}, EnemyKind::Zombie);
    ls::EnemySteer::Steer(registry, {0, 0});

    const auto &transform =
        registry.GetEntitiesByGroup("enemies")[0]
            .GetComponent<storm::TransformComponent>();
    // From (100,100) player at (0,0), facing up-left: negative angle on the
    // clockwise-from-+X convention (SDL +Y is down).
    Assert::That(transform.rotation, Is().LessThan(0.0f));
  };

  It(zeroes_velocity_when_the_enemy_is_on_top_of_the_player) {
    Registry registry;
    ls::SpawnEnemy(registry, {0, 0}, EnemyKind::Zombie);
    ls::EnemySteer::Steer(registry, {0, 0});

    const auto &rb = registry.GetEntitiesByGroup("enemies")[0]
        .GetComponent<storm::RigidBodyComponent>();
    // The zombie is exactly on top — velocity must be zero, not NaN.
    Assert::That(rb.velocity.x, EqualsWithDelta(0.0f, 1e-3f));
    Assert::That(rb.velocity.y, EqualsWithDelta(0.0f, 1e-3f));
  };
};