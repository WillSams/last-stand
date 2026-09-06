#pragma once

#include <cstddef>
#include <vector>

#include <glm/glm.hpp>
#include <stormengine2/ecs.h>

// A fixed-size pool of bullets, created once with their full component set and
// never killed. Firing claims a slot; expiry returns it. Bullets are pooled
// rather than CreateEntity/Kill'd per shot for two reasons: the ECS logs every
// creation and component add (console I/O in the hot path), and a firing stream
// is exactly the churn the pool exists to amortise.
//
// Inactive bullets park one per 128px step from parkOrigin. The broadphase
// buckets bodies by position; hundreds parked at one point land in one cell
// and pair with each other — the O(n^2) the grid exists to prevent.
class BulletPool {
public:
    BulletPool(storm::Registry &registry, std::size_t count,
               glm::vec2 parkOrigin);

    // Claims a bullet and sends it from `from` along `angleDeg` at `speed`
    // px/s for up to `ttl` seconds. Returns false when the pool is exhausted —
    // the caller does not queue.
    bool Fire(glm::vec2 from, float angleDeg, float speed, float ttl, int damage = 1);

    // Ages every active bullet; releases the expired. Call once per frame.
    void Tick(double dt);

    // Returns a bullet to its park slot and the free-list. Idempotent in
    // effect: releasing an inactive bullet is a no-op (its index is not on
    // the free-list twice) because the caller drives from Tick's scan or a
    // contact that was just applied this frame.
    void Release(storm::Entity bullet);

    int ActiveCount() const;
    std::size_t FreeCount() const { return free_.size(); }

    // Exposed for specs only: the entity list is stable across Fire/Tick.
    const std::vector<storm::Entity> &Bullets() const { return bullets_; }

private:
    storm::Registry &registry_;
    std::vector<storm::Entity> bullets_;
    std::vector<std::size_t> free_;     // bullet indices available to Fire
    glm::vec2 parkOrigin_;
    float parkStep_ = 128.0f;           // one bullet per broadphase cell
};