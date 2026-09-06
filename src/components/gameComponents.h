#pragma once

#include <stormengine2/ecs.h>

// Game-owned components, carried on entities alongside the engine's built-ins.
// Each derives from storm::Component<T> (CRTP) so that Component<T>::GetId()
// assigns it a process-unique id the same way the engine's own components are
// assigned one — see the RockIdentity pattern in salvage-run.

struct BulletComponent : storm::Component<BulletComponent> {
    // false while parked; the pair filter skips inert bullets.
    bool active = false;
    float ttl = 0.0f;  // seconds until released back to the pool
    int damage = 1;
};

struct HealthComponent : storm::Component<HealthComponent> {
    int hp = 5;
    int max = 5;
    float sinceHit = 0.0f;  // seconds since last damage; drives regen
};

struct TeamComponent : storm::Component<TeamComponent> {
    enum Side { Player, Enemy };
    Side side = Enemy;

    // An explicit constructor, because a base-class member cannot be
    // aggregate-initialised in C++17 and TeamComponent{TeamComponent::Player}
    // must compile.
    explicit TeamComponent(Side s = Enemy) : side(s) {}
};

struct BrainComponent : storm::Component<BrainComponent> {
    float speed = 60.0f;  // melee chase speed in world px/s
};

// Damage dealt to the player on a contact body hit (melee enemies) or per
// bullet hit. Kept off HealthComponent so the responder can read it without
// upcasting.
struct ContactDamageComponent : storm::Component<ContactDamageComponent> {
    int damage = 1;
};