#include "playState.h"

#include "../arena/camera.h"
#include "../combat/bulletPool.h"
#include "../combat/pickups.h"
#include "../combat/regen.h"
#include "../input/move.h"
#include "../arena/walls.h"
#include "../hud/minimap.h"
#include "../spawn/director.h"
#include "../components/gameComponents.h"
#include "../entities/factory.h"
#include "../input/aim.h"
#include "../systems/brain.h"

const std::string PlayState::s_playID = "PLAY_STATE";

// Player speed in world px/s, and the survivor1_gun.png sprite size (51x43 —
// NOT 64x64; the Kenney characters are variable-size and the engine source
// rect rule applies).
constexpr float kPlayerSpeed = 120.0f;
constexpr int kPlayerW = 51;
constexpr int kPlayerH = 43;

// Firing: one round every fire interval; 128 bullets pooled (a firing stream
// dies long before the pool does). Fire returns false when exhausted, so a
// saturated trigger is silent rather than queueing.
constexpr float kFireInterval = 0.15f;
constexpr std::size_t kBulletCount = 128;
constexpr float kBulletSpeed = 700.0f;
constexpr float kBulletTtl = 0.9f;

// Park unused bullets far off-screen so the broadphase never pairs parked
// bullets with live ones.
constexpr glm::vec2 kBulletPark{-20000.0f, 0.0f};

using last_stand::EnemyKind;
using last_stand::EnemySpecFor;
using last_stand::SpawnEnemy;
using last_stand::EnemySteer;


PlayState::PlayState(SDL_Renderer *renderer, int windowWidth, int windowHeight,
                     bool isDebugging, AssetStore_Ptr assetStore,
                     bool &isRunning)
    : renderer_(renderer), windowWidth_(windowWidth),
      windowHeight_(windowHeight), isDebugging_(isDebugging),
      assetStore_(std::move(assetStore)), isRunning_(isRunning) {}

PlayState::~PlayState() {}

// Initialize in onEnter(), not the constructor. changeState() calls onEnter()
// after pushing the state; clean() calls onExit() before deleting it.
bool PlayState::onEnter() {
    LoadAssets();
    scoreBoard_ = scoreStore_.Load();

    // Build the arena before entities so the player spawns inside a solid
    // world and can resolve against it the same frame.
    arena_ = std::make_unique<Arena>("./assets/maps/arena.map", 64);

    // Bullets are created (and parked) before the first registry_.Update()
    // admits them. Parking far off-screen stops them being rendered on the
    // first camera frame.
    bullets_ = std::make_unique<BulletPool>(registry_, kBulletCount,
                                            glm::vec2(-20000.0f, 0.0f));

    // Register systems BEFORE creating entities. AddSystem<T>() only constructs
    // and registers -- it never scans existing entities, so a system added
    // after the entities were flushed starts empty and stays empty.
    registry_.AddSystem<MovementSystem>();
    registry_.AddSystem<RenderSystem>();
    registry_.AddSystem<ContactSystem>();

    // Filter before the manifold: bullets never pair with each other or with
    // an inactive (parked) bullet. Enemy-vs-enemy is deliberately kept so a
    // crowd reads as a crowd (v1 steering then separates them).
    registry_.GetSystem<ContactSystem>().SetPairFilter(
        [](const Entity &a, const Entity &b) {
            const auto *ba = a.TryGetComponent<BulletComponent>();
            const auto *bb = b.TryGetComponent<BulletComponent>();
            if (ba && bb)
                return false;                      // bullets never pair
            if (ba && !ba->active)
                return false;                      // parked bullets are inert
            if (bb && !bb->active)
                return false;
            return true;
        });

    SpawnPlayer();
    SpawnTestEnemies();

    gamepad_.OpenFirstAttached();
    if (gamepad_.Connected()) logger_.Log("Gamepad connected: " + gamepad_.Name());
    else logger_.Log("Gamepad: none attached (keyboard only)");

    millisecondsPreviousFrame_ = SDL_GetTicks();
    return true;
}

void PlayState::SpawnTestEnemies() {
    // A couple of enemies up front so the contact rules have something to act
    // on during Task 4's verification window. The ramp replaces this in Task 5.
    SpawnEnemy(registry_, {600.0f, 300.0f}, EnemyKind::Zombie);
    SpawnEnemy(registry_, {800.0f, 500.0f}, EnemyKind::Robot);
    SpawnEnemy(registry_, {300.0f, 700.0f}, EnemyKind::Old);
}

// onExit() must be idempotent -- it can run twice (state machine call, then
// destructor).
bool PlayState::onExit() {
    gamepad_.Shutdown();
    if (assetStore_) {
        assetStore_->ClearAssets();
    }
    return true;
}

void PlayState::LoadAssets() {
    // survivor1_gun.png is 51x43, drawn facing right, rotated toward the mouse
    // by Transform::rotation (see update()). GetTexture returns nullptr for a
    // missing id rather than throwing, so a missing file is silent at the
    // point of use; check here, where the path is still in scope.
    assetStore_->AddTexture(renderer_, "player",
                            "./assets/gfx/PNG/Survivor 1/survivor1_gun.png");
    if (!assetStore_->GetTexture("player")) {
        logger_.Err("Missing ./assets/gfx/PNG/Survivor 1/survivor1_gun.png "
                    "-- run from the game root.");
    }

    // The arena tilesheet, referenced as "tiles" by every line in arena.map.
    assetStore_->AddTexture(renderer_, "tiles",
                            "./assets/gfx/Tilesheet/tilesheet_complete.png");
    if (!assetStore_->GetTexture("tiles")) {
        logger_.Err("Missing ./assets/gfx/Tilesheet/tilesheet_complete.png "
                    "-- run from the game root.");
    }

    assetStore_->AddTexture(renderer_, "bullet", "./assets/gfx/bullet.png");
    assetStore_->AddTexture(renderer_, "weapon_gun", "./assets/gfx/PNG/weapon_gun.png");
    assetStore_->AddTexture(renderer_, "weapon_machine", "./assets/gfx/PNG/weapon_machine.png");
    assetStore_->AddTexture(renderer_, "weapon_silencer", "./assets/gfx/PNG/weapon_silencer.png");
    assetStore_->AddTexture(renderer_, "player_machine", "./assets/gfx/PNG/Survivor 1/survivor1_machine.png");
    assetStore_->AddTexture(renderer_, "player_silencer", "./assets/gfx/PNG/Survivor 1/survivor1_silencer.png");
    storm::LightingOverlay::Params lp;
    lp.width = windowWidth_; lp.height = windowHeight_;
    lp.radius = 260.0f; lp.keyOpacity = 56; lp.vignetteOpacity = 120;
    lighting_.Build(renderer_, lp);
    assetStore_->AddFont("hud", "./assets/fonts/kenvector_future.ttf", 24);
    assetStore_->AddFont("hudBig", "./assets/fonts/kenvector_future.ttf", 48);
    if (!assetStore_->GetFont("hud") || !assetStore_->GetFont("hudBig")) {
        logger_.Err("Missing ./assets/fonts/kenvector_future.ttf -- run from the game root.");
    }
    if (!assetStore_->GetTexture("bullet")) {
        logger_.Err("Missing ./assets/gfx/bullet.png -- run from the game root.");
    }

    // Enemy cast. Each is a `hold` pose (not `gun`): v1 enemies are melee.
    // The zombie file keeps the upstream typo: zoimbie1_hold.png.
    assetStore_->AddTexture(renderer_, "zombie",
                            "./assets/gfx/PNG/Zombie 1/zoimbie1_hold.png");
    if (!assetStore_->GetTexture("zombie")) {
        logger_.Err("Missing ./assets/gfx/PNG/Zombie 1/zoimbie1_hold.png");
    }
    assetStore_->AddTexture(renderer_, "robot",
                            "./assets/gfx/PNG/Robot 1/robot1_hold.png");
    if (!assetStore_->GetTexture("robot")) {
        logger_.Err("Missing ./assets/gfx/PNG/Robot 1/robot1_hold.png");
    }
    assetStore_->AddTexture(renderer_, "old",
                            "./assets/gfx/PNG/Man Old/manOld_hold.png");
    if (!assetStore_->GetTexture("old")) {
        logger_.Err("Missing ./assets/gfx/PNG/Man Old/manOld_hold.png");
    }
}

void PlayState::SpawnPlayer() {
    Entity player = registry_.CreateEntity();
    player.Tag("player");
    player.AddComponent<TransformComponent>(glm::vec2(400.0f, 300.0f),
                                            glm::vec2(1.0f, 1.0f), 0.0);
    player.AddComponent<RigidBodyComponent>(glm::vec2(0.0f, 0.0f));
    // survivor1_gun.png is 51x43, not 64x64. SpriteComponent width/height are
    // the SOURCE rect, so they must be the image's own dimensions — a 64x64
    // src rect here walks off the texture and the sprite draws nothing.
    // zIndex 2 so the player draws above the arena floor.
    player.AddComponent<SpriteComponent>("player", kPlayerW, kPlayerH, 2);
    // A circle collider's offset places the CENTRE: {25, 21} centres a 21px
    // radius on the 51x43 sprite drawn from its top-left.
    player.AddComponent<CircleColliderComponent>(21.0f, glm::vec2(25, 21));
    // Health + damage are on the player too: an enemy that walks into the
    // player must know how many hits it takes and how much it deals.
    HealthComponent playerHealth;
    playerHealth.hp = 5;
    playerHealth.max = 5;
    player.AddComponent<HealthComponent>(playerHealth);
    // The player deals a fixed contact hit per bullet — enemies read only
    // ContactDamageComponent on contact, so the bullet pool does the real
    // damage anyway. The value here is a defensive default (unused by v1).
    player.AddComponent<ContactDamageComponent>(ContactDamageComponent{});
    player.AddComponent<TeamComponent>(TeamComponent{TeamComponent::Player});

    player_ = player;
}

// Walls position-resolve via the free helper, not a per-tile collider, and
// it applies to every entity that has a BodySizeDescriptor (player, enemies,
// dead bullets — none of them may ever overlap a solid tile).
static void ResolveAgainstArena(Entity &body, int w, int h, Arena &arena) {
    auto &transform = body.GetComponent<TransformComponent>();
    auto &rb = body.GetComponent<RigidBodyComponent>();
    last_stand::ResolveAgainstWalls(transform.position, rb.velocity, w, h,
                                    arena.TileSize(),
                                    [](int col, int row, void *ctx) {
                                        return static_cast<Arena *>(ctx)
                                            ->IsSolid(col, row);
                                    },
                                    &arena);
}

void PlayState::ResolvePlayerWalls() {
    if (!player_ || !arena_)
        return;
    ResolveAgainstArena(*player_, kPlayerW, kPlayerH, *arena_);
}

void PlayState::ResolveEnemiesAgainstWalls() {
    if (!arena_)
        return;
    for (const Entity &enemy : registry_.GetEntitiesByGroup("enemies")) {
        // Sprite bounds come from the entity's own SpriteComponent, not a spec
        // lookup: the sprite is the only durable truth about how wide the body
        // actually is on-screen.
        const auto &sprite = enemy.GetComponent<SpriteComponent>();
        ResolveAgainstArena(const_cast<Entity &>(enemy),
                            sprite.width, sprite.height, *arena_);
    }
}

// Walk ContactSystem's current frame and apply the game's meaning: bullets
// damage enemies; enemies damage the player in contact; enemy-vs-enemy
// pressure separates the swarm. The rule all live here, keyed on the
// components each body has — a contact is a list of statements, not a
// hierarchy of conditions.
// saboteur caught two bugs in review: enemies could go forever-negative HP
// without dying, and the player lost 60 HP/sec while a body touched them.
constexpr float kRegenDelay = 3.0f;
constexpr float kRegenRate = 1.0f;
constexpr float kPlayerInvulnerabilitySeconds = 0.5f;

namespace {

// An enemy is dead the moment its HP hits zero. The engine's Kill is deferred
// to registry_.Update(), so a dead body exists for the rest of the frame
// — the same frame the contact was applied on.
bool UpdateDeaths(storm::Registry &registry, const char *group) {
    bool anyDead = false;
    for (const storm::Entity &entity : registry.GetEntitiesByGroup(group)) {
        const auto &health =
            entity.GetComponent<HealthComponent>();
        if (health.hp <= 0) {
            auto pos = entity.GetComponent<storm::TransformComponent>().position;
            // 1 in 8 deaths drops a pickup; RollDrop picks the kind. Health is
            // in that table now (see pickups.h) — it used to be unreachable.
            static unsigned dropSeed = 987;
            dropSeed = dropSeed * 1664525u + 1013904223u;
            last_stand::PickupKind kind;
            if (last_stand::RollDrop(dropSeed, kind))
                last_stand::SpawnPickup(registry, pos, kind);
            registry.KillEntity(entity);
            anyDead = true;
        }
    }
    return anyDead;
}

} // namespace

void PlayState::ApplyContacts() {
    auto &contactSystem = registry_.GetSystem<ContactSystem>();
    for (const storm::Contact &contact : contactSystem.GetContacts()) {
        const Entity &a = contact.a;
        const Entity &b = contact.b;

        const auto *teamsA = a.TryGetComponent<TeamComponent>();
        const auto *teamsB = b.TryGetComponent<TeamComponent>();
        const auto *bulletA = a.TryGetComponent<BulletComponent>();
        const auto *bulletB = b.TryGetComponent<BulletComponent>();

        // Bullet-vs-Enemy: damage the enemy, release the bullet. The death
        // sweep runs below in this same frame: hp<=0 here does not delay.
        if (bulletA && teamsB && teamsB->side == TeamComponent::Enemy) {
            auto &enemyHealth = b.GetComponent<HealthComponent>().hp;
            auto &bulletDamage = a.GetComponent<BulletComponent>().damage;
            enemyHealth -= bulletDamage;
            bullets_->Release(a);
        }
        if (bulletB && teamsA && teamsA->side == TeamComponent::Enemy) {
            auto &enemyHealth = a.GetComponent<HealthComponent>().hp;
            auto &bulletDamage = b.GetComponent<BulletComponent>().damage;
            enemyHealth -= bulletDamage;
            bullets_->Release(b);
        }

        // Enemy-vs-Enemy: the swarm spreads apart.
        if (teamsA && teamsB && teamsA->side == TeamComponent::Enemy
            && teamsB->side == TeamComponent::Enemy) {
            const glm::vec2 push = storm::ContactSystem::MinimumTranslation(contact);
            auto &posA = a.GetComponent<TransformComponent>().position;
            auto &posB = b.GetComponent<TransformComponent>().position;
            posA -= push * 0.5f;
            posB += push * 0.5f;
        }

        // Enemy-vs-Player: damage with a per-hit cooldown, not every frame.
        // Dead player takes no more hits: hp clamps at 0, bar reads empty.
        if (teamsA && teamsB && teamsA->side != teamsB->side && player_ && !dead_) {
            const bool aIsPlayer = (teamsA->side == TeamComponent::Player);
            const Entity &player = aIsPlayer ? a : b;
            const Entity &enemy = aIsPlayer ? b : a;

            if (player == *player_) {
                auto &health = player_->GetComponent<HealthComponent>();
                const auto &damage =
                    enemy.GetComponent<ContactDamageComponent>();
                if (health.sinceHit >= kPlayerInvulnerabilitySeconds) {
                    health.hp = std::max(0, health.hp - damage.damage);
                    health.sinceHit = 0.0f;
                    regenCarry_ = 0.0f;
                }
            }
        }

        // Player-vs-Pickup: consume, apply effect. Pickups carry no Team,
        // so match on player handle directly, not on team pair.
        const auto *pkA = a.TryGetComponent<last_stand::PickupComponent>();
        const auto *pkB = b.TryGetComponent<last_stand::PickupComponent>();
        if (pkA || pkB) {
            const Entity &pk = pkA ? a : b;
            const Entity &other = pkA ? b : a;
            if (player_ && other == *player_) {
                const auto kind = const_cast<Entity &>(pk).GetComponent<last_stand::PickupComponent>().kind;
                if (kind == last_stand::PickupKind::Health) {
                    auto &h = player_->GetComponent<HealthComponent>();
                    h.hp = h.max;
                } else {
                    auto st = last_stand::StatsFor(kind);
                    fireInterval_ = st.interval; bulletDamage_ = st.damage;
                    player_->GetComponent<SpriteComponent>().assetId = st.pose;
                }
                registry_.KillEntity(pk);
            }
        }
    }
}



// The active state owns ALL event polling. Never call SDL_PollEvent in both
// Game::ProcessInput and a state's processInput -- the queue is shared.
void PlayState::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        gamepad_.HandleEvent(event);
        switch (event.type) {
        case SDL_QUIT:
            isRunning_ = false;
            return;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                isRunning_ = false;
                return;
            }
            if (event.key.keysym.sym == SDLK_a) moveLeft_  = true;
            if (event.key.keysym.sym == SDLK_d) moveRight_ = true;
            if (event.key.keysym.sym == SDLK_w) moveUp_    = true;
            if (event.key.keysym.sym == SDLK_s) moveDown_  = true;
            break;
        case SDL_KEYUP:
            if (event.key.keysym.sym == SDLK_a) moveLeft_  = false;
            if (event.key.keysym.sym == SDLK_d) moveRight_ = false;
            if (event.key.keysym.sym == SDLK_w) moveUp_    = false;
            if (event.key.keysym.sym == SDLK_s) moveDown_  = false;
            break;
        case SDL_MOUSEMOTION:
            aim_ = glm::vec2(static_cast<float>(event.motion.x),
                             static_cast<float>(event.motion.y));
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT)
                firing_ = true;
            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT)
                firing_ = false;
            break;
        default:
            break;
        }
    }
}

void PlayState::update() {
    // Variable dt with a 60 FPS cap. Nothing enforces a minimum frame rate.
    //
    // On engine v1.3.0+ this whole block, and the millisecondsPreviousFrame_
    // member shadowing GameState's own, collapse to:
    //
    //     const double deltaTime = CapFrameRate();
    //
    // which also clamps a hitch. It is spelled out here because the scaffold
    // compiles against any 1.x install; see README.md, "Engine version".
    int wait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecondsPreviousFrame_);
    if (wait > 0 && wait <= MILLISECS_PER_FRAME) {
        SDL_Delay(wait);
    }
    double deltaTime = (SDL_GetTicks() - millisecondsPreviousFrame_) / 1000.0;
    millisecondsPreviousFrame_ = SDL_GetTicks();

    // Flush deferred entity adds/kills FIRST, before running any system.
    // Entity creation and destruction are batched, not immediate.
    registry_.Update();

    gamepad_.Update();

    if (player_) {
        auto &rb = player_->GetComponent<RigidBodyComponent>();
        auto &transform = player_->GetComponent<TransformComponent>();

        // Twin-stick: left stick moves (PlayerSteering merges WASD/stick/d-pad),
        // right stick aims when deflected, trigger/A fires.
        last_stand::MoveInput mi;
        mi.left = moveLeft_; mi.right = moveRight_;
        mi.up = moveUp_; mi.down = moveDown_;
        const auto &pad = gamepad_.Current();
        mi.stickX = pad.leftX; mi.stickY = pad.leftY;
        if (gamepad_.Connected()) {
            mi.dpadLeft = gamepad_.Down(storm::GamepadButton::Left);
            mi.dpadRight = gamepad_.Down(storm::GamepadButton::Right);
            mi.dpadUp = gamepad_.Down(storm::GamepadButton::Up);
            mi.dpadDown = gamepad_.Down(storm::GamepadButton::Down);
        }
        rb.velocity = last_stand::PlayerSteering(mi, kPlayerSpeed);

        float aimDeg;
        if (gamepad_.Connected() && glm::length(glm::vec2(pad.rightX, pad.rightY)) > 0.3f) {
            aimDeg = AimAngleDegrees(glm::vec2(0, 0), glm::vec2(pad.rightX, pad.rightY));
            padAim_ = true; padAimDeg_ = aimDeg;
        } else if (padAim_ && !gamepad_.Connected()) {
            padAim_ = false;
        } else if (gamepad_.Connected() && glm::length(glm::vec2(pad.rightX, pad.rightY)) <= 0.3f) {
            padAim_ = false;
        }
        if (padAim_) {
            transform.rotation = padAimDeg_;
        } else {
            const glm::vec2 mouseWorld =
                aim_ + glm::vec2(static_cast<float>(camera_.x),
                                 static_cast<float>(camera_.y));
            transform.rotation = AimAngleDegrees(transform.position, mouseWorld);
        }
        if (gamepad_.Connected())
            padFiring_ = (pad.triggerRight > 0.5f || gamepad_.Down(storm::GamepadButton::A));
        else
            padFiring_ = false;
    }

    // Each concrete system declares its own non-virtual Update with a bespoke
    // signature. There is no scheduler -- the state calls them by name, in an
    // order it chooses.
    registry_.GetSystem<MovementSystem>().Update(deltaTime);

    // Steer enemies before wall resolution so velocities are current.
    if (player_) {
        const glm::vec2 playerPos =
            player_->GetComponent<TransformComponent>().position;
        EnemySteer::Steer(registry_, playerPos);
    }

    // Firing, after movement so a new bullet measures from the updated
    // transform. Fire returns false when exhausted; a failing shot is
    // silent — a jammed trigger is the correct behaviour.
    fireCooldown_ -= static_cast<float>(deltaTime);
    if ((firing_ || padFiring_) && fireCooldown_ <= 0.0f && player_ && !dead_) {
        const auto &transform = player_->GetComponent<TransformComponent>();
        const glm::vec2 muzzle = transform.position +
                                 glm::vec2(kPlayerW * 0.5f, kPlayerH * 0.5f);
        float angle = padAim_ ? padAimDeg_ : AimAngleDegrees(
            muzzle, aim_ + glm::vec2(static_cast<float>(camera_.x),
                                     static_cast<float>(camera_.y)));
        if (bullets_->Fire(muzzle, angle, kBulletSpeed, kBulletTtl, bulletDamage_)) {}
        fireCooldown_ = fireInterval_;
    }

    bullets_->Tick(deltaTime);

    // Regen + spawn ramp clocks.
    elapsed_ += static_cast<float>(deltaTime);
    if (player_) {
        auto &h = player_->GetComponent<HealthComponent>();
        h.sinceHit += static_cast<float>(deltaTime);
        // ponytail: carry fractional regen across frames; int hp + per-frame
        // floor would never cross 1hp at 60fps
        if (h.sinceHit > kRegenDelay && h.hp > 0 && h.hp < h.max) {
            regenCarry_ += kRegenRate * static_cast<float>(deltaTime);
            if (regenCarry_ >= 1.0f) {
                int whole = static_cast<int>(regenCarry_);
                h.hp = std::min(h.max, h.hp + whole);
                regenCarry_ -= static_cast<float>(whole);
            }
        } else {
            regenCarry_ = 0.0f;
        }
        if (h.hp <= 0 && !dead_) {
            dead_ = true;
            deadTime_ = elapsed_;
            // The score IS the survival time. Submit once, on the death edge,
            // and persist only when it is a new best.
            newBest_ = last_stand::Submit(scoreBoard_, static_cast<int>(deadTime_));
            if (newBest_) scoreStore_.Save(scoreBoard_);
        }
    }
    // Continuous spawn: budget accumulator, never burst-catch-up (cap 3).
    if (!dead_) {
        spawnBudget_ += last_stand::SpawnsPerSecond(elapsed_) * static_cast<float>(deltaTime);
        if (spawnBudget_ > 3.0f) spawnBudget_ = 3.0f;
        while (spawnBudget_ >= 1.0f) {
            spawnBudget_ -= 1.0f;
            auto sp = last_stand::SpawnPointOutside((float)camera_.x, (float)camera_.y, (float)camera_.w, (float)camera_.h, arena_->PixelSize().x, arena_->PixelSize().y, 64.0f, spawnSeed_);
            last_stand::EnemyKind k = last_stand::EnemyKind::Zombie;
            if (elapsed_ > 240.0f) { spawnSeed_ = spawnSeed_ * 1664525u + 1013904223u; k = (spawnSeed_ % 3 == 0) ? last_stand::EnemyKind::Old : ((spawnSeed_ % 3 == 1) ? last_stand::EnemyKind::Robot : last_stand::EnemyKind::Zombie); }
            else if (elapsed_ > 120.0f) { spawnSeed_ = spawnSeed_ * 1664525u + 1013904223u; k = (spawnSeed_ % 2) ? last_stand::EnemyKind::Robot : last_stand::EnemyKind::Zombie; }
            SpawnEnemy(registry_, {sp.x, sp.y}, k);
        }
    }

    // Wall resolution before contacts: a body that resolves out of a wall
    // stays out of any contact pair for the frame.
    ResolvePlayerWalls();
    ResolveEnemiesAgainstWalls();

    // ContactSystem narrows to a candidate set and reports; the contact
    // resolution pass decides what each pair means for v1 in contacts_.
    registry_.GetSystem<ContactSystem>().Update();
    ApplyContacts();
    // Separation shoves enemies into walls the pre-contact pass just
    // resolved them out of. Resolve again so a crowd against a wall
    // rests on it instead of squeezing through.
    ResolveEnemiesAgainstWalls();

    // Death runs after contacts so a bullet released this frame was still a
    // valid fire. The tombstone pass runs after damage, so it never gets
    // caught between a bullet's last score and the next registry sweep.
    UpdateDeaths(registry_, "enemies");
    // Restart on R (or gamepad Start/A) after death.
    if (dead_) {
        bool retry = false;
        SDL_PumpEvents();
        const Uint8 *ks = SDL_GetKeyboardState(nullptr);
        if (ks[SDL_SCANCODE_R]) retry = true;
        if (gamepad_.Connected() &&
            (storm::GamepadPressed(gamepad_.Current(), gamepad_.Previous(), storm::GamepadButton::Start) ||
             storm::GamepadPressed(gamepad_.Current(), gamepad_.Previous(), storm::GamepadButton::A)))
            retry = true;
        if (retry && player_) {
            auto &h = player_->GetComponent<HealthComponent>();
            h.hp = h.max; h.sinceHit = 99.0f;
            player_->GetComponent<TransformComponent>().position = {400, 300};
            elapsed_ = 0; spawnBudget_ = 0; dead_ = false;
            fireInterval_ = 0.15f;
            newBest_ = false;
            player_->GetComponent<SpriteComponent>().assetId = "player";
        }
    }

    // Camera follows the player, clamped to the arena. CameraTopLeft is pure
    // (no SDL); the SDL_Rect the renderer wants is assembled here.
    if (player_) {
        const auto &transform = player_->GetComponent<TransformComponent>();
        const glm::ivec2 topLeft = CameraTopLeft(transform.position,
                                                 windowWidth_, windowHeight_,
                                                 arena_->PixelSize());
        camera_.x = topLeft.x;
        camera_.y = topLeft.y;
        camera_.w = windowWidth_;
        camera_.h = windowHeight_;
    }
}

void PlayState::render() {
    SDL_SetRenderDrawColor(renderer_, 21, 21, 21, 255);
    SDL_RenderClear(renderer_);

    // The arena draws first, as the floor. Pass the camera so the world
    // scrolls; RenderSystem's sprites use the same camera below.
    if (arena_) {
        arena_->Draw(renderer_, *assetStore_, camera_);
    }

    registry_.GetSystem<RenderSystem>().Update(renderer_, *assetStore_,
                                               &camera_);

    // Health pickups carry no sprite (the pack ships no health icon), so draw
    // them as a green cross in world space, before lighting so the vignette
    // dims them like every other pickup.
    if (registry_.DoesGroupExist("pickups")) {
        for (const auto &e : registry_.GetEntitiesByGroup("pickups")) {
            const auto *pk = e.TryGetComponent<last_stand::PickupComponent>();
            if (!pk || pk->kind != last_stand::PickupKind::Health) continue;
            const glm::vec2 p = e.GetComponent<TransformComponent>().position;
            const int x = static_cast<int>(p.x) - camera_.x - 6;
            const int y = static_cast<int>(p.y) - camera_.y - 6;
            SDL_SetRenderDrawColor(renderer_, 34, 197, 94, 255);  // health green
            const SDL_Rect vertical{x + 4, y, 4, 12};
            const SDL_Rect horizontal{x, y + 4, 12, 4};
            SDL_RenderFillRect(renderer_, &vertical);
            SDL_RenderFillRect(renderer_, &horizontal);
        }
    }

    lighting_.Draw(renderer_);
    // HUD + minimap AFTER lighting or vignette dims them.
    {
        // Health bar top-left.
        if (player_) {
            auto &h = player_->GetComponent<HealthComponent>();
            float f = 0.0f;
            if (h.max > 0 && h.hp > 0) f = (float)h.hp / (float)h.max;
            if (f < 0.0f) f = 0.0f;
            if (f > 1.0f) f = 1.0f;
            SDL_Rect bg{10, 10, 200, 18};
            SDL_SetRenderDrawColor(renderer_, 60, 0, 0, 255);
            SDL_RenderFillRect(renderer_, &bg);
            SDL_Rect fg{10, 10, (int)(200 * f), 18};
            SDL_SetRenderDrawColor(renderer_, 200, 30, 30, 255);
            SDL_RenderFillRect(renderer_, &fg);
        }
        // Minimap bottom-right 140x105.
        last_stand::MiniPanel mp{(float)windowWidth_ - 150.0f, (float)windowHeight_ - 115.0f, 140.0f, 105.0f};
        SDL_Rect panel{(int)mp.x, (int)mp.y, (int)mp.w, (int)mp.h};
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 200);
        SDL_RenderFillRect(renderer_, &panel);
        glm::vec2 ws = arena_ ? arena_->PixelSize() : glm::vec2(1, 1);
        auto dot = [&](glm::vec2 w, int r, Uint8 R, Uint8 G, Uint8 B) {
            glm::vec2 m = last_stand::WorldToMinimap(w, ws, mp);
            SDL_Rect d{(int)m.x - r, (int)m.y - r, r * 2, r * 2};
            SDL_SetRenderDrawColor(renderer_, R, G, B, 255);
            SDL_RenderFillRect(renderer_, &d);
        };
        if (player_) dot(player_->GetComponent<TransformComponent>().position, 2, 0, 255, 0);
        for (auto &e : registry_.GetEntitiesByGroup("enemies")) dot(e.GetComponent<TransformComponent>().position, 2, 255, 0, 0);
        if (registry_.DoesGroupExist("pickups"))
            for (auto &e : registry_.GetEntitiesByGroup("pickups")) {
                const auto *pk = e.TryGetComponent<last_stand::PickupComponent>();
                const glm::vec2 p = e.GetComponent<TransformComponent>().position;
                if (pk && pk->kind == last_stand::PickupKind::Health)
                    dot(p, 2, 34, 197, 94);   // health green, matches the cross
                else
                    dot(p, 2, 255, 255, 0);   // weapon yellow
            }
        if (dead_) {
            // Death overlay: dim the world, then a panel sized to its own
            // text so no line can collide at any point size.
            SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 160);
            SDL_Rect dim{0, 0, windowWidth_, windowHeight_};
            SDL_RenderFillRect(renderer_, &dim);
            SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_NONE);

            TTF_Font *big = assetStore_->GetFont("hudBig");
            TTF_Font *hud = assetStore_->GetFont("hud");
            const std::string title = "YOU DIED";
            const std::string survived =
                "Survived " + last_stand::FormatClock(static_cast<int>(deadTime_));
            const std::string best =
                newBest_ ? std::string("NEW BEST!")
                         : "Best " + last_stand::FormatClock(scoreBoard_.bestSeconds);
            const std::string retry = "Press R / A to retry";

            const int gap = 8;
            const int pad = 20;
            const int contentH = storm::Text::Measure(big, title).y + gap +
                                 storm::Text::Measure(hud, survived).y + gap +
                                 storm::Text::Measure(hud, best).y + 2 * gap +
                                 storm::Text::Measure(hud, retry).y;
            const SDL_Rect box{windowWidth_ / 2 - 190,
                               windowHeight_ / 2 - (contentH + 2 * pad) / 2, 380,
                               contentH + 2 * pad};
            SDL_SetRenderDrawColor(renderer_, 20, 0, 0, 255);
            SDL_RenderFillRect(renderer_, &box);
            SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer_, &box);

            const SDL_Color white{255, 255, 255, 255};
            const SDL_Color grey{180, 180, 180, 255};
            const SDL_Color gold{251, 191, 36, 255};  // ITCH gold: new best only
            const int cx = windowWidth_ / 2;
            int lineY = box.y + pad;
            auto line = [&](TTF_Font *font, const std::string &text, SDL_Color c) {
                lineY += storm::Text::DrawCentred(renderer_, font, text, cx, lineY, c).y + gap;
            };
            line(big, title, white);
            line(hud, survived, white);
            line(hud, best, newBest_ ? gold : grey);
            lineY += gap;  // a little extra air before the prompt
            line(hud, retry, white);
        } else if (player_) {
            // Run clock + best, top-right, right-aligned to a 20px margin so a
            // growing minute count never walks off the edge. The clock uses the
            // same mm:ss format as the death screen.
            TTF_Font *hud = assetStore_->GetFont("hud");
            const std::string clock =
                last_stand::FormatClock(static_cast<int>(elapsed_));
            const SDL_Point clockSize = storm::Text::Measure(hud, clock);
            storm::Text::Draw(renderer_, hud, clock, windowWidth_ - 20 - clockSize.x,
                              10, SDL_Color{255, 255, 255, 255});
            const std::string best =
                "BEST " + last_stand::FormatClock(scoreBoard_.bestSeconds);
            const SDL_Point bestSize = storm::Text::Measure(hud, best);
            storm::Text::Draw(renderer_, hud, best, windowWidth_ - 20 - bestSize.x,
                              42, SDL_Color{180, 180, 180, 255});
        }
    }

    // Both take a camera, and both default to world coordinates without one.
    // Pass &camera to each together, or the outlines drift away from the
    // sprites the moment the view scrolls.
    if (isDebugging_) {
        registry_.GetSystem<RenderColliderSystem>().Update(renderer_, &camera_);
    }

    SDL_RenderPresent(renderer_);
}
