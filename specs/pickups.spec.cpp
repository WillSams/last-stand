#include <igloo/igloo_alt.h>

#include <stormengine2/components/circleCollider.h>
#include <stormengine2/components/sprite.h>

#include "../src/combat/pickups.h"
#include "../src/entities/factory.h"

using namespace igloo;
using last_stand::PickupKind;
using last_stand::RollDrop;
namespace ls = last_stand;
using storm::Registry;

Describe(PickupDropSpec) {

  It(drops_only_on_a_multiple_of_eight) {
    PickupKind kind;
    Assert::That(RollDrop(7u, kind), IsFalse());
    Assert::That(RollDrop(9u, kind), IsFalse());
    Assert::That(RollDrop(8u, kind), IsTrue());
  };

  It(can_drop_health) {
    // The bug this fixes: the old selector was `roll % 3`, which can never
    // name PickupKind::Health (3), so health pickups never spawned and the
    // full-heal branch in ApplyContacts was dead.
    bool sawHealth = false;
    for (unsigned roll = 0; roll < 4096 && !sawHealth; ++roll) {
      PickupKind kind;
      if (RollDrop(roll, kind) && kind == PickupKind::Health) sawHealth = true;
    }
    Assert::That(sawHealth, IsTrue());
  };

  It(drops_more_than_one_kind) {
    // The gate and the selector must not share the low bits: picking with
    // `roll % 4` would always name Gun, because a drop is `roll % 8 == 0`.
    bool sawGun = false, sawOther = false;
    for (unsigned roll = 0; roll < 4096; ++roll) {
      PickupKind kind;
      if (!RollDrop(roll, kind)) continue;
      if (kind == PickupKind::Gun) sawGun = true;
      else                         sawOther = true;
    }
    Assert::That(sawGun, IsTrue());
    Assert::That(sawOther, IsTrue());
  };

  It(never_names_a_kind_outside_the_drop_table) {
    for (unsigned roll = 0; roll < 4096; ++roll) {
      PickupKind kind;
      if (!RollDrop(roll, kind)) continue;
      const bool inTable = kind == PickupKind::Gun ||
                           kind == PickupKind::Machine ||
                           kind == PickupKind::Silencer ||
                           kind == PickupKind::Health;
      Assert::That(inTable, IsTrue());
    }
  };
};

Describe(PickupSpawnSpec) {

  It(spawns_health_as_a_sprite_less_pickup) {
    // The health pickup has no art in the pack: it must carry a PickupComponent
    // and a collider, and NO SpriteComponent, or playState's green cross would
    // draw on top of a weapon sprite.
    Registry registry;
    ls::SpawnPickup(registry, {10, 20}, PickupKind::Health);
    const storm::Entity e = registry.GetEntitiesByGroup("pickups")[0];

    const auto *pickup = e.TryGetComponent<ls::PickupComponent>();
    Assert::That(pickup != nullptr, IsTrue());
    Assert::That(pickup->kind == PickupKind::Health, IsTrue());
    Assert::That(e.TryGetComponent<storm::SpriteComponent>() == nullptr, IsTrue());
    Assert::That(e.TryGetComponent<storm::CircleColliderComponent>() != nullptr,
                 IsTrue());
  };

  It(spawns_a_weapon_with_a_sprite) {
    Registry registry;
    ls::SpawnPickup(registry, {0, 0}, PickupKind::Gun);
    const storm::Entity e = registry.GetEntitiesByGroup("pickups")[0];

    Assert::That(e.TryGetComponent<storm::SpriteComponent>() != nullptr, IsTrue());
    const auto *pickup = e.TryGetComponent<ls::PickupComponent>();
    Assert::That(pickup != nullptr && pickup->kind == PickupKind::Gun, IsTrue());
  };
};
