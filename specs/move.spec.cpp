#include <igloo/igloo_alt.h>
#include "../src/input/move.h"
using namespace igloo;
using namespace last_stand;
namespace { bool Near(float a, float b) { return std::abs(a - b) < 1e-4f; } }
Describe(SteeringSpec) {
  It(maps_wasd) {
    MoveInput in; in.right = true;
    Assert::That(Near(PlayerSteering(in, 120.0f).x, 120.0f), Is().True());
  };
  It(is_proportional_on_stick) {
    MoveInput in; in.stickX = 0.5f;
    Assert::That(Near(PlayerSteering(in, 120.0f).x, 60.0f), Is().True());
  };
  It(clamps_diagonal_to_speed) {
    MoveInput in; in.stickX = 1.0f; in.stickY = 1.0f;
    Assert::That(Near(glm::length(PlayerSteering(in, 120.0f)), 120.0f), Is().True());
  };
  It(ignores_dpad_when_stick_live) {
    MoveInput in; in.stickX = 1.0f; in.dpadRight = true;
    Assert::That(Near(PlayerSteering(in, 120.0f).x, 120.0f), Is().True());
  };
  It(uses_dpad_when_stick_dead) {
    MoveInput in; in.dpadUp = true;
    Assert::That(Near(PlayerSteering(in, 120.0f).y, -120.0f), Is().True());
  };
};
