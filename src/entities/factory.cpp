#include "factory.h"

#include <stormengine2/components/circleCollider.h>
#include <stormengine2/components/rigidBody.h>
#include <stormengine2/components/sprite.h>
#include <stormengine2/components/transform.h>

#include "../components/gameComponents.h"

namespace last_stand {

storm::Entity SpawnEnemy(storm::Registry &registry, glm::vec2 at,
                         EnemyKind kind, float speedScale) {
    const EnemySpec &spec = EnemySpecFor(kind);

    storm::Entity enemy = registry.CreateEntity();
    enemy.Group("enemies");
    enemy.AddComponent<storm::TransformComponent>(at, glm::vec2(1.0f, 1.0f),
                                                  0.0);
    enemy.AddComponent<storm::RigidBodyComponent>(glm::vec2(0.0f, 0.0f));
    // A collider whose offset places its CENTRE — half the sprite dimensions,
    // not half the radius.
    enemy.AddComponent<storm::CircleColliderComponent>(
        spec.radius, glm::vec2(spec.width * 0.5f, spec.height * 0.5f));
    enemy.AddComponent<storm::SpriteComponent>(spec.textureId, spec.width,
                                               spec.height, /*z=*/2);
    // HP/damage/speed all come from the spec table; defaults alone would
    // silently equalise every enemy type.
    HealthComponent health;
    health.hp = spec.hp;
    health.max = spec.hp;
    enemy.AddComponent<HealthComponent>(health);
    ContactDamageComponent contactDamage;
    contactDamage.damage = spec.damage;
    enemy.AddComponent<ContactDamageComponent>(contactDamage);
    enemy.AddComponent<TeamComponent>(TeamComponent{TeamComponent::Enemy});
    BrainComponent brain;
    brain.speed = spec.speed * speedScale;
    enemy.AddComponent<BrainComponent>(brain);
    return enemy;
}

static const char *PickupTexture(PickupKind kind) {
    switch (kind) {
        case PickupKind::Machine: return "weapon_machine";
        case PickupKind::Silencer: return "weapon_silencer";
        default: return "weapon_gun";
    }
}

storm::Entity SpawnPickup(storm::Registry &registry, glm::vec2 at, PickupKind kind) {
    storm::Entity e = registry.CreateEntity();
    e.Group("pickups");
    e.AddComponent<storm::TransformComponent>(at, glm::vec2(1.0f, 1.0f), 0.0);
    e.AddComponent<storm::RigidBodyComponent>(glm::vec2(0.0f, 0.0f));
    if (kind == PickupKind::Health) {
        // No health icon ships with the pack, so the health pickup carries no
        // sprite: playState draws it as a green cross (see render()). The
        // collider still gives it the same pickup radius as a weapon.
        e.AddComponent<storm::CircleColliderComponent>(12.0f, glm::vec2(6.0f, 6.0f));
        e.AddComponent<PickupComponent>(PickupComponent{kind});
        return e;
    }
    int w = 19, h = 10;
    if (kind == PickupKind::Machine) { w = 33; h = 10; }
    if (kind == PickupKind::Silencer) { w = 25; h = 10; }
    e.AddComponent<storm::CircleColliderComponent>(12.0f, glm::vec2(w * 0.5f, h * 0.5f));
    e.AddComponent<storm::SpriteComponent>(PickupTexture(kind), w, h, 1);
    e.AddComponent<PickupComponent>(PickupComponent{kind});
    return e;
}

} // namespace last_stand