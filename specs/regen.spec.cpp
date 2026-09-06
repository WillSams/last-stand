#include <igloo/igloo_alt.h>

#include <cmath>

#include "../src/combat/regen.h"

using namespace igloo;
using last_stand::RegenTick;

Describe(RegenSpec) {

  It(does_nothing_inside_the_regen_delay) {
    // 3 hp, 0.2s since last hit, delay 0.5 — the hit clock is too fresh.
    Assert::That(RegenTick(3, 0.2f, 1.0f, 0.5f, 1.0f), Equals(3.0f));
  };

  It(does_nothing_at_exactly_the_delay) {
    // Boundary: exactly at the delay is still inside the cooldown window.
    Assert::That(RegenTick(3, 0.5f, 1.0f, 0.5f, 1.0f), Equals(3.0f));
  };

  It(accrues_full_hp_past_the_delay) {
    // 0.5s since hit (delay passed), dt of 1s at 1/s → one full HP.
    Assert::That(RegenTick(3, 0.6f, 1.0f, 0.5f, 1.0f), Equals(4.0f));
  };

  It(returns_to_int_below_1_hp) {
    // A fractional regen that never crosses an integer boundary leaves the
    // hp unchanged — otherwise every frame's sub-HP growth is a soft hit on
    // a rounding error.
    Assert::That(RegenTick(3, 0.6f, 0.4f, 0.5f, 1.0f), Equals(3.0f));
  };

  It(nevers_goes_negative) {
    // Regen on 0 (already dead) always stays dead.
    Assert::That(RegenTick(0, 0.6f, 1.0f, 0.5f, 1.0f), Equals(0.0f));
  };
};