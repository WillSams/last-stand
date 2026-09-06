#include <igloo/igloo_alt.h>

#include <cmath>

#include "../src/input/aim.h"

using namespace igloo;

namespace {

// SDL screen space: +Y is down, rotation is clockwise from +X.
constexpr float kEps = 1e-3f;

} // namespace

Describe(AimAngleSpec) {

  It(returns_zero_when_aiming_directly_right) {
    Assert::That(AimAngleDegrees({0, 0}, {100, 0}),
                 EqualsWithDelta(0.0f, kEps));
  };

  It(returns_180_when_aiming_directly_left) {
    Assert::That(AimAngleDegrees({0, 0}, {-100, 0}),
                 EqualsWithDelta(180.0f, kEps));
  };

  It(returns_90_when_aiming_down) {
    // +Y is down, so "below" is +90 degrees.
    Assert::That(AimAngleDegrees({0, 0}, {0, 100}),
                 EqualsWithDelta(90.0f, kEps));
  };

  It(returns_negative_90_when_aiming_up) {
    Assert::That(AimAngleDegrees({0, 0}, {0, -100}),
                 EqualsWithDelta(-90.0f, kEps));
  };

  It(returns_45_for_a_diagonal_to_the_lower_right) {
    Assert::That(AimAngleDegrees({0, 0}, {100, 100}),
                 EqualsWithDelta(45.0f, kEps));
  };

  It(is_origin_independent) {
    // The angle depends on the delta, not on the absolute positions.
    Assert::That(AimAngleDegrees({500, 300}, {600, 300}),
                 EqualsWithDelta(AimAngleDegrees({10, 20}, {110, 20}), kEps));
  };

  It(returns_zero_instead_of_NaN_when_from_equals_to) {
    // atan2(0, 0) is undefined; the aim axis is vertical here so the value
    // must be finite rather than NaN, which would propagate into rotation.
    const float a = AimAngleDegrees({50, 50}, {50, 50});
    Assert::That(std::isfinite(a), Is().True());
    Assert::That(a, EqualsWithDelta(0.0f, kEps));
  };
};