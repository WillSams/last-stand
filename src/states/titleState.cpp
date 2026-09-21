#include "titleState.h"

#include "playState.h"

#include "../input/menu.h"
#include "../score/score.h"

const std::string TitleState::s_titleID = "TITLE_STATE";

TitleState::TitleState(SDL_Renderer *renderer, int windowWidth, int windowHeight,
                       bool isDebugging, AssetStore_Ptr assetStore,
                       bool &isRunning, GameStateMachine *stateMachine)
    : renderer_(renderer), windowWidth_(windowWidth),
      windowHeight_(windowHeight), isDebugging_(isDebugging),
      assetStore_(std::move(assetStore)), isRunning_(isRunning),
      stateMachine_(stateMachine) {}

TitleState::~TitleState() {}

bool TitleState::onEnter() {
    // Same font file as the HUD, one bigger id for the title. AddFont needs
    // TTF_Init, which Game::Initialize has already called.
    assetStore_->AddFont("title", "./assets/fonts/kenvector_future.ttf", 64);
    assetStore_->AddFont("hud", "./assets/fonts/kenvector_future.ttf", 24);
    if (!assetStore_->GetFont("title") || !assetStore_->GetFont("hud")) {
        logger_.Err("Missing ./assets/fonts/kenvector_future.ttf -- run from "
                    "the game root.");
    }

    bestSeconds_ = last_stand::ScoreStore{"./last-stand.sav"}.Load().bestSeconds;

    gamepad_.OpenFirstAttached();
    return true;
}

// Idempotent, and safe after StartGame: moving the store out leaves
// assetStore_ null, so the ClearAssets here becomes a no-op and PlayState
// keeps the assets it was handed.
bool TitleState::onExit() {
    gamepad_.Shutdown();
    if (assetStore_) {
        assetStore_->ClearAssets();
    }
    return true;
}

void TitleState::StartGame() {
    // Pass the machine's address through, not a reference to this state: by
    // the time the new state runs, this one is defunct.
    stateMachine_->changeState(
        new PlayState(renderer_, windowWidth_, windowHeight_, isDebugging_,
                      std::move(assetStore_), isRunning_));
}

void TitleState::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        gamepad_.HandleEvent(event);
        switch (event.type) {
        case SDL_QUIT:
            quitKey_ = true;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) quitKey_ = true;
            if (event.key.keysym.sym == SDLK_RETURN ||
                event.key.keysym.sym == SDLK_KP_ENTER ||
                event.key.keysym.sym == SDLK_SPACE)
                startKey_ = true;
            break;
        default:
            break;
        }
    }
}

void TitleState::update() {
    // Paces the frame and sleeps out the budget; the title screen has no
    // simulation, so the delta is not needed.
    CapFrameRate();
    gamepad_.Update();

    last_stand::MenuInput in;
    in.enter = startKey_;
    in.esc = quitKey_;
    if (gamepad_.Connected()) {
        in.padStart = storm::GamepadPressed(gamepad_.Current(),
                                            gamepad_.Previous(),
                                            storm::GamepadButton::Start);
        in.padA = storm::GamepadPressed(gamepad_.Current(),
                                        gamepad_.Previous(),
                                        storm::GamepadButton::A);
    }

    switch (last_stand::MenuActionFor(in)) {
    case last_stand::MenuAction::Quit:
        isRunning_ = false;
        return;
    case last_stand::MenuAction::Start:
        StartGame();
        return;
    case last_stand::MenuAction::None:
    default:
        break;
    }
}

void TitleState::render() {
    SDL_SetRenderDrawColor(renderer_, 21, 21, 21, 255);
    SDL_RenderClear(renderer_);

    const SDL_Color white{255, 255, 255, 255};
    const SDL_Color grey{180, 180, 180, 255};
    const SDL_Color gold{251, 191, 36, 255};
    const int cx = windowWidth_ / 2;

    storm::Text::DrawCentred(renderer_, assetStore_->GetFont("title"),
                             "LAST STAND", cx, windowHeight_ / 2 - 130, white);
    storm::Text::DrawCentred(renderer_, assetStore_->GetFont("hud"),
                             "Kite the horde. Shoot what closes in.",
                             cx, windowHeight_ / 2 - 40, grey);
    storm::Text::DrawCentred(renderer_, assetStore_->GetFont("hud"),
                             "Press Enter / A to start", cx,
                             windowHeight_ / 2 + 20, white);
    storm::Text::DrawCentred(renderer_, assetStore_->GetFont("hud"),
                             "Esc to quit", cx, windowHeight_ / 2 + 60, grey);
    if (bestSeconds_ > 0) {
        storm::Text::DrawCentred(
            renderer_, assetStore_->GetFont("hud"),
            "Best " + last_stand::FormatClock(bestSeconds_), cx,
            windowHeight_ / 2 + 120, gold);
    }

    SDL_RenderPresent(renderer_);
}
