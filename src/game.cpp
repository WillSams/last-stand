#include "game.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

Game::Game() {
    assetStore = std::make_unique<AssetStore>();
    logger     = std::make_unique<Logger>();
}

Game::~Game() {}

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        logger->Err("Error initializing SDL.");
        return;
    }

    // AssetStore::AddFont needs this; without it font loads return nullptr
    // and HUD text silently draws nothing.
    if (TTF_Init() != 0) {
        logger->Err("Error initializing SDL_ttf.");
        return;
    }

    window = SDL_CreateWindow("Last Stand",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              windowWidth, windowHeight, 0);
    if (!window) {
        logger->Err("Error creating SDL window.");
        return;
    }

    // Window/taskbar icon. Non-fatal if absent.
    IMG_Init(IMG_INIT_PNG);
    if (SDL_Surface *icon = IMG_Load("./assets/gfx/icon.png")) {
        SDL_SetWindowIcon(window, icon);
        SDL_FreeSurface(icon);
    } else {
        logger->Log("icon.png not loaded; window keeps default icon");
    }

    renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        logger->Err("Error creating SDL renderer.");
        return;
    }

    // The state machine owns every state pointer -- pass `new`-allocated states
    // and never delete them yourself. assetStore is moved into the first state;
    // pass a raw pointer or reference to any state after this one.
    gameStateMachine.changeState(
        new PlayState(renderer, windowWidth, windowHeight, isDebugging,
                      std::move(assetStore), isRunning));

    isRunning = true;
}

void Game::Run() {
    Initialize();
    while (isRunning) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::ProcessInput() { gameStateMachine.processInput(); }
void Game::Update()       { gameStateMachine.update(); }
void Game::Render()       { gameStateMachine.render(); }

void Game::Destroy() {
    // ClearAssets before TTF_Quit/SDL_Quit: AssetStore owns fonts that
    // TTF_CloseFont frees.
    gameStateMachine.clean();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
    SDL_Quit();
}
