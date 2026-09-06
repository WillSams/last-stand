#include <igloo/igloo_alt.h>

#include <stormengine2/components/rigidBody.h>

#include "../src/combat/bulletPool.h"
#include "../src/components/gameComponents.h"

using namespace igloo;
using storm::Registry;

namespace {
constexpr glm::vec2 kPark{0.0f, -10000.0f};
constexpr float kEps = 1e-3f;
} // namespace

Describe(BulletPoolSpec) {

  It(claims_and_releases) {
    Registry registry;
    BulletPool pool(registry, 8, kPark);
    Assert::That(pool.FreeCount(), Equals(8));

    Assert::That(pool.Fire({0, 0}, 0.0f, 100.0f, 1.0f), Is().True());
    Assert::That(pool.FreeCount(), Equals(7));

    // Fire a second, let the first expire, then check the slot returns.
    Assert::That(pool.Fire({0, 0}, 90.0f, 100.0f, 1.0f), Is().True());
    Assert::That(pool.ActiveCount(), Equals(2));

    pool.Tick(1.5);
    Assert::That(pool.ActiveCount(), Equals(0));
    Assert::That(pool.FreeCount(), Equals(8));
  };

  It(returns_false_when_exhausted) {
    Registry registry;
    BulletPool pool(registry, 2, kPark);
    Assert::That(pool.Fire({0, 0}, 0.0f, 100.0f, 60.0f), Is().True());
    Assert::That(pool.Fire({0, 0}, 0.0f, 100.0f, 60.0f), Is().True());
    Assert::That(pool.Fire({0, 0}, 0.0f, 100.0f, 60.0f), Is().False());
    Assert::That(pool.FreeCount(), Equals(0));
  };

  It(fires_bullets_along_the_angle) {
    Registry registry;
    BulletPool pool(registry, 1, kPark);
    pool.Fire({10, 10}, 90.0f, 200.0f, 60.0f);
    // The engine registry is lazily admitted; velocity was written at Fire
    // time on the stored entity regardless.
    for (const auto &b : pool.Bullets()) {
      const auto &rb = b.GetComponent<storm::RigidBodyComponent>();
      Assert::That(rb.velocity.x, EqualsWithDelta(0.0f, kEps));
      Assert::That(rb.velocity.y, EqualsWithDelta(200.0f, kEps));
    }
  };

  It(reuse_is_LIFO_within_the_free_list) {
    // Whole point of checking: release returns a slot, and the next Fire
    // reclaims exactly that one — exercise the cycle several times.
    Registry registry;
    BulletPool pool(registry, 2, kPark);
    pool.Fire({0, 0}, 0.0f, 10.0f, 0.1f);
    pool.Tick(0.2);
    Assert::That(pool.FreeCount(), Equals(2));
    Assert::That(pool.Fire({0, 0}, 0.0f, 10.0f, 60.0f), Is().True());
    Assert::That(pool.ActiveCount(), Equals(1));
  };

  It(reclaims_only_exact_slots_mid_flight) {
    Registry registry;
    BulletPool pool(registry, 3, kPark);
    pool.Fire({0, 0}, 0.0f, 10.0f, 0.02f);   // first, will expire
    pool.Fire({0, 0}, 0.0f, 10.0f, 60.0f);   // second, runs long
    pool.Tick(0.03f);                        // first expired
    Assert::That(pool.FreeCount(), Equals(2));

    pool.Fire({5, 5}, 0.0f, 10.0f, 60.0f);   // claims the expired slot back
    Assert::That(pool.FreeCount(), Equals(1));

    pool.Fire({5, 5}, 0.0f, 10.0f, 60.0f);   // claims the third slot
    Assert::That(pool.FreeCount(), Equals(0));

    // Exhausted: the very next Fire must not claim an in-flight slot.
    Assert::That(pool.Fire({0, 0}, 0.0f, 10.0f, 60.0f), Is().False());
    Assert::That(pool.FreeCount(), Equals(0));
  };

  It(does_not_double_release) {
    // A release on an already-inactive bullet must not push its index onto
    // the free-list a second time.
    Registry registry;
    BulletPool pool(registry, 1, kPark);
    pool.Fire({0, 0}, 0.0f, 10.0f, 0.1f);
    pool.Tick(0.2);
    Assert::That(pool.FreeCount(), Equals(1));
    pool.Tick(1.0);  // already expired; must not release again
    Assert::That(pool.FreeCount(), Equals(1));
  };
};