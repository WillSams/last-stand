#pragma once

#include <SDL2/SDL.h>

#include <stormengine2/gameStateMachine.h>
#include <stormengine2/input/gamepad.h>
#include <stormengine2/states/gameState.h>
#include <stormengine2/text.h>

#include <memory>
#include <string>

using namespace storm;

// The title screen: the first state the game enters. It owns the AssetStore
// until the player starts, then moves it into PlayState, which owns it for the
// rest of the run.
//
// This is the only state that needs the machine: it starts the game by
// changing state. PlayState never changes state (it restarts in place), so it
// does not take the pointer.
class TitleState : public GameState {
public:
    TitleState(SDL_Renderer *renderer, int windowWidth, int windowHeight,
               bool isDebugging, AssetStore_Ptr assetStore, bool &isRunning,
               GameStateMachine *stateMachine);
    ~TitleState();

    void processInput() override;
    void update() override;
    void render() override;
    bool onEnter() override;
    bool onExit() override;

    std::string getStateID() const override { return s_titleID; }

private:
    void StartGame();

    static const std::string s_titleID;

    SDL_Renderer  *renderer_;
    int            windowWidth_;
    int            windowHeight_;
    bool           isDebugging_;
    AssetStore_Ptr assetStore_;
    Logger         logger_;
    bool          &isRunning_;

    // Game owns the machine; this is a borrowed pointer, never deleted here.
    GameStateMachine *stateMachine_ = nullptr;

    // Set by processInput, consumed by update: keyboard start and quit are
    // edges, and update is where the pad edge is available too.
    bool startKey_ = false;
    bool quitKey_ = false;

    storm::Gamepad gamepad_;

    // Best run from a previous session, shown under the prompt.
    int bestSeconds_ = 0;
};
