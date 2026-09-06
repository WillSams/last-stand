#include <igloo/igloo_alt.h>

#include "../src/arena/walls.h"

using namespace igloo;
namespace ls = last_stand;

namespace {

// 5x5 test map, 64-px tiles. Only the border ring is solid:
//
//   row 0: tiles (0..4, 0) all solid
//   row 4: solid
//   col 0, col 4: solid
//   (2, 2) is OPEN: the box interior a body can move through.
bool TestSolid(int col, int row, void *) {
    return row == 0 || row == 4 || col == 0 || col == 4;
}

// Snap a 64-wide, 43-tall body through the resolver.
void Snap(glm::vec2 &pos, glm::vec2 &vel) {
    last_stand::ResolveAgainstWalls(pos, vel, 64, 43, 64, TestSolid, nullptr);
}

} // namespace

Describe(WallResolveSpec) {

  It(stops_a_body_moving_right_into_the_right_wall) {
    // Body is 64 wide; colliding face is its right edge. Moving right into
    // col 4 (solid) while still inside col 3: the resolver snaps back so the
    // right edge touches x = 4*64 = 256, which happens at left = 256-64 = 192.
    glm::vec2 pos{200.0f, 128.0f};
    glm::vec2 vel{100.0f, 0.0f};
    Snap(pos, vel);
    Assert::That(pos.x, Equals(192.0f));
    Assert::That(vel.x, EqualsWithDelta(0.0f, 1e-3f));
    Assert::That(vel.y, EqualsWithDelta(0.0f, 1e-3f));
  };

  It(stops_a_body_moving_left_into_the_left_wall) {
    // Moving left into col 0 (solid) at left=20: resolve places the left
    // edge on the right side of the wall tile, x = 64.
    glm::vec2 pos{20.0f, 128.0f};
    glm::vec2 vel{-100.0f, 0.0f};
    Snap(pos, vel);
    Assert::That(pos.x, Equals(64.0f));
    Assert::That(vel.x, EqualsWithDelta(0.0f, 1e-3f));
  };

  It(stops_a_body_moving_down_into_the_floor) {
    // Bottom tile row (row 4) is solid; moving down at y=270 (bottom edge
    // 270+43 = 313, well into col-violating row 4) — resolve pushes the body
    // up so bottom = 4*64 = 256, so top y = 256 - 43 = 213.
    glm::vec2 pos{128.0f, 270.0f};
    glm::vec2 vel{0.0f, 100.0f};
    Snap(pos, vel);
    Assert::That(pos.y, Equals(213.0f));
    Assert::That(vel.y, EqualsWithDelta(0.0f, 1e-3f));
  };

  It(stops_a_body_moving_up_into_the_ceiling) {
    glm::vec2 pos{128.0f, 20.0f};
    glm::vec2 vel{0.0f, -100.0f};
    Snap(pos, vel);
    Assert::That(pos.y, Equals(64.0f));
    Assert::That(vel.y, EqualsWithDelta(0.0f, 1e-3f));
  };

  It(does_nothing_when_the_body_never_touches_a_wall) {
    glm::vec2 pos{128.0f, 128.0f};
    glm::vec2 vel{50.0f, 25.0f};
    Snap(pos, vel);
    Assert::That(pos.x, EqualsWithDelta(128.0f, 1e-3f));
    Assert::That(pos.y, EqualsWithDelta(128.0f, 1e-3f));
    Assert::That(vel.x, EqualsWithDelta(50.0f, 1e-3f));
    Assert::That(vel.y, EqualsWithDelta(25.0f, 1e-3f));
  };

  It(does_not_bite_at_zero_velocity) {
    // A stationary body standing in open space should never teleport.
    glm::vec2 pos{128.0f, 128.0f};
    glm::vec2 vel{0.0f, 0.0f};
    Snap(pos, vel);
    Assert::That(pos.x, Equals(128.0f));
    Assert::That(pos.y, Equals(128.0f));
  };
};