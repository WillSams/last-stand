#include <igloo/igloo_alt.h>
#include "../src/spawn/director.h"
#include "../src/hud/minimap.h"
#include "../src/combat/pickups.h"
using namespace igloo;
using namespace last_stand;
Describe(DirectorSpec) {
  It(rises_monotonically_and_nonzero_at_start) {
    Assert::That(SpawnsPerSecond(0.0f) > 0.0f, Is().True());
    Assert::That(SpawnsPerSecond(60.0f) > SpawnsPerSecond(10.0f), Is().True());
    Assert::That(SpawnsPerSecond(300.0f) > SpawnsPerSecond(60.0f), Is().True());
  };
  It(spawn_point_stays_inside_world) {
    unsigned s = 42;
    for (int i = 0; i < 20; ++i) {
      auto p = SpawnPointOutside(500, 400, 800, 600, 2368, 1792, 64, s);
      Assert::That(p.x >= 0 && p.x < 2368, Is().True());
      Assert::That(p.y >= 0 && p.y < 1792, Is().True());
    }
  };
};
Describe(MinimapSpec) {
  It(maps_corners_and_clamps) {
    MiniPanel p{10, 10, 100, 80};
    auto a = WorldToMinimap({0, 0}, {1000, 800}, p);
    Assert::That(a.x, Equals(10.0f)); Assert::That(a.y, Equals(10.0f));
    auto b = WorldToMinimap({1000, 800}, {1000, 800}, p);
    Assert::That(b.x, Equals(110.0f)); Assert::That(b.y, Equals(90.0f));
    auto c = WorldToMinimap({-50, 9999}, {1000, 800}, p);
    Assert::That(c.x, Equals(10.0f)); Assert::That(c.y, Equals(90.0f));
  };
};
Describe(PickupsSpec) {
  It(machine_fires_faster) {
    Assert::That(StatsFor(PickupKind::Machine).interval < StatsFor(PickupKind::Gun).interval, Is().True());
    Assert::That(StatsFor(PickupKind::Silencer).damage == 2, Is().True());
  };
};
