#include "bulletPool.h"

#include <algorithm>
#include <cmath>

#include <stormengine2/components/circleCollider.h>
#include <stormengine2/components/rigidBody.h>
#include <stormengine2/components/sprite.h>
#include <stormengine2/components/transform.h>

#include "../components/gameComponents.h"
#include "../input/aim.h"

namespace {

// A bullet's own sprite is 16x16; the circle collider sits on its centre.
constexpr float kBulletRadius = 5.0f;
constexpr glm::vec2 kBulletColliderOffset{8.0f, 8.0f};
constexpr int kBulletZIndex = 1;

} // namespace

using storm::Entity;

BulletPool::BulletPool(storm::Registry &registry, std::size_t count,
                       glm::vec2 parkOrigin)
    : registry_(registry), parkOrigin_(parkOrigin) {
    bullets_.reserve(count);
    free_.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        // Park one per cell, spread along X: the broadphase buckets by
        // position, so stacking every inactive bullet on one point would pair
        // them with each other each frame.
        const glm::vec2 park = parkOrigin_ + glm::vec2(i * parkStep_, 0.0f);

        Entity bullet = registry_.CreateEntity();
        bullet.AddComponent<storm::TransformComponent>(park,
                                                       glm::vec2(1.0f, 1.0f),
                                                       0.0);
        bullet.AddComponent<storm::RigidBodyComponent>(glm::vec2(0.0f, 0.0f));
        bullet.AddComponent<storm::SpriteComponent>("bullet", 16, 16,
                                                    kBulletZIndex);
        bullet.AddComponent<storm::CircleColliderComponent>(
            kBulletRadius, kBulletColliderOffset);
        bullet.AddComponent<BulletComponent>();
        // v1 enemies are melee, so only the player fires: every bullet in the
        // pool is Player side for its whole life.
        bullet.AddComponent<TeamComponent>(TeamComponent{TeamComponent::Player});
        bullets_.push_back(bullet);
        free_.push_back(i);
    }
}

bool BulletPool::Fire(glm::vec2 from, float angleDeg, float speed,
                      float ttl, int damage) {
    if (free_.empty())
        return false;

    const std::size_t i = free_.back();
    free_.pop_back();
    Entity bullet = bullets_[i];

    auto &transform = bullet.GetComponent<storm::TransformComponent>();
    auto &rigid = bullet.GetComponent<storm::RigidBodyComponent>();
    auto &bulletComp = bullet.GetComponent<BulletComponent>();

    transform.position = from;
    // Sprite faces right at 0 degrees, matching the Kenney characters.
    transform.rotation = static_cast<double>(angleDeg);

    const float radians = angleDeg * kPi / 180.0f;
    rigid.velocity =
        glm::vec2(std::cos(radians), std::sin(radians)) * speed;

    bulletComp.active = true;
    bulletComp.ttl = ttl;
    bulletComp.damage = damage;
    return true;
}

void BulletPool::Tick(double dt) {
    for (Entity bullet : bullets_) {
        auto &comp = bullet.GetComponent<BulletComponent>();
        if (!comp.active)
            continue;
        comp.ttl -= static_cast<float>(dt);
        if (comp.ttl <= 0.0f)
            Release(bullet);
    }
}

void BulletPool::Release(Entity bullet) {
    // Locate the owned slot by handle. Entity equality compares id AND
    // generation, so this finds the pool slot the handle came from. (Took the
    // slot index via &bullet - bullets_.data() once; a by-value parameter is
    // never a vector element address, so that always computed garbage.)
    const auto it = std::find(bullets_.begin(), bullets_.end(), bullet);
    if (it == bullets_.end())
        return;  // not one of ours
    const std::size_t i =
        static_cast<std::size_t>(std::distance(bullets_.begin(), it));

    auto &comp = it->GetComponent<BulletComponent>();
    // Guard against double-release: an inactive bullet's index must not be on
    // the free-list twice, or two Fire calls would claim the same bullet.
    if (!comp.active)
        return;

    comp.active = false;
    comp.ttl = 0.0f;

    auto &rigid = it->GetComponent<storm::RigidBodyComponent>();
    rigid.velocity = glm::vec2(0.0f, 0.0f);

    auto &transform = it->GetComponent<storm::TransformComponent>();
    transform.position =
        parkOrigin_ + glm::vec2(static_cast<float>(i) * parkStep_, 0.0f);

    free_.push_back(i);
}

int BulletPool::ActiveCount() const {
    return static_cast<int>(bullets_.size() - free_.size());
}