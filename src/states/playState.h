#pragma once

#include <SDL2/SDL.h>

#include <stormengine2/states/gameState.h>
#include <stormengine2/lighting.h>
#include <stormengine2/input/gamepad.h>
#include <stormengine2/text.h>

#include <glm/glm.hpp>

#include <memory>
#include <optional>
#include <string>

#include "../arena/arena.h"
#include "../combat/bulletPool.h"
#include "../score/score.h"

using namespace storm;

// gameState.h already pulls in SDL2, ecs.h, assetStore.h, logger.h and every
// built-in component and system. Do not re-include them here.
//
// (That transitive include is a documented defect -- KNOWN_ISSUES #8, ~713
// headers to declare a 23-line interface -- and goes away in 2.0.0. Include what
// you use in headers of your own.)

class PlayState : public GameState {
public:
    PlayState(SDL_Renderer *renderer, int windowWidth, int windowHeight,
              bool isDebugging, AssetStore_Ptr assetStore, bool &isRunning);
    ~PlayState();

    void processInput() override;
    void update() override;
    void render() override;
    bool onEnter() override;
    bool onExit() override;

    std::string getStateID() const override { return s_playID; }

private:
    void LoadAssets();
    void SpawnPlayer();
    void SpawnTestEnemies();
    void ResolvePlayerWalls();
    void ResolveEnemiesAgainstWalls();
    void ApplyContacts();

    static const std::string s_playID;

    SDL_Renderer  *renderer_;
    int            windowWidth_;
    int            windowHeight_;
    bool           isDebugging_;
    AssetStore_Ptr assetStore_;
    Logger         logger_;

    // A state stops the loop by writing to the bool& the Game handed it.
    // There is no engine quit API.
    bool &isRunning_;

    Registry registry_;

    // The arena owns the tilemap and the solidity grid the player resolves
    // against. Built in onEnter before the player spawns.
    std::unique_ptr<Arena> arena_;
    std::unique_ptr<BulletPool> bullets_;

    // Camera top-left in world pixels, recomputed each frame from the player.
    SDL_Rect camera_ = {0, 0, 0, 0};

    // Held in an optional rather than a bare Entity member: Entity has no
    // default constructor at all -- only Entity(std::size_t) -- so a bare
    // `Entity player_;` member does not compile. optional gives it a real empty
    // state. (A hand-built Entity(id) is not UB on v1.2.2+ either: its registry
    // pointer is null-initialised and every forwarder null-checks and no-ops.
    // It still does nothing useful.)
    //
    // This is safe here only because nothing in the scaffold kills the player.
    // Entity ids are recycled and Entity carries no generation, so once your
    // game can destroy the player, stop caching the handle and look it up by
    // tag each frame instead -- guarded with registry_.DoesTagExist("player"),
    // which needs v1.2.2 or newer.
    std::optional<Entity> player_;

    // WASD movement, tracked as held-key booleans in processInput and read
    // into velocity in update().
    bool moveLeft_  = false;
    bool moveRight_ = false;
    bool moveUp_    = false;
    bool moveDown_  = false;

    // Mouse position in screen pixels, fed by SDL_MOUSEMOTION each frame. The
    // camera follows the player, so aim is resolved in world space by adding
    // the camera offset (see update()).
    glm::vec2 aim_ = glm::vec2(0.0f, 0.0f);

    // Left mouse held. Aiming is continuous; firing is rate-limited by
    // fireCooldown_.
    bool  firing_ = false;
    float fireCooldown_ = 0.0f;
    float fireInterval_ = 0.15f;
    int bulletDamage_ = 1;
    float elapsed_ = 0.0f;
    float spawnBudget_ = 0.0f;
    unsigned spawnSeed_ = 12345;
    bool dead_ = false;
    float deadTime_ = 0.0f;
    // Best run, persisted across sessions. Loaded in onEnter, submitted on
    // death; the score IS the survival time.
    last_stand::ScoreStore scoreStore_{"./last-stand.sav"};
    last_stand::ScoreBoard scoreBoard_;
    bool newBest_ = false;
    float regenCarry_ = 0.0f;
    storm::Gamepad gamepad_;
    bool padAim_ = false;
    float padAimDeg_ = 0.0f;
    bool padFiring_ = false;
    storm::LightingOverlay lighting_;

    int millisecondsPreviousFrame_ = 0;
};
