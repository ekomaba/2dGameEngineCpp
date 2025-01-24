#include <fstream>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"

Game::Game() {
    isRunning = false;
    
    registry = std::make_unique<Registry>();
    assetStore = std::make_unique<AssetStore>();
    Logger::Log("Game Constructor called!");
}

Game::~Game() {
    Logger::Log("Game Destructor called!");
}

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        Logger::Err("Error initializing SDL.");
        return;
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

#ifdef DEBUG
    // Debug mode window 800x600
    windowWidth = 800;
    windowHeight = 600;
#else
    // Release mode window fullscreen
    windowWidth = displayMode.w;
    windowHeight = displayMode.h;
#endif

    // Creating window
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
#ifdef DEBUG
    // Window mode debug
    SDL_WINDOW_SHOWN
#else
    // Window mode relase
    SDL_WINDOW_FULLSCREEN
#endif
);

    if (!window) {
        Logger::Err("Error creating SDL Window.");
        return;
    }
    
    // Creating Renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        Logger::Err("Error creating SDL Renderer window.");
        return;
    }

    isRunning = true;

}

void Game::Run() {
    Setup();
    while(isRunning) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::Destroy() {
    Logger::Log("Destroying the game instance!");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::ProcessInput() {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        switch(sdlEvent.type) {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
                    isRunning = false;
                }
                break;
        }
    }
}

void Game::LoadLevel(int level) {
    // Adding Systems
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();

    // Adding Assets
    assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
    assetStore->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");

    // Load the tilemap
    int tileSize = 32;
    double tileScale = 2.0;
    int mapNumCols = 25;
    int mapNumRows = 20;

    std::fstream mapFile;
    mapFile.open("./assets/tilemaps/jungle.map");

    for (int y = 0; y < mapNumRows; y++) {
        for (int x = 0; x < mapNumCols; x++) {
            char ch;
            mapFile.get(ch);
            int srcRectY = std::atoi(&ch) * tileSize;
            mapFile.get(ch);
            int srcRectX = std::atoi(&ch) * tileSize;
            mapFile.ignore();

            Entity tile = registry->CreateEntity();
            tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0);
            tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, srcRectX, srcRectY);
        }
    }

    mapFile.close();

    Entity tank = registry->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(50.0, 50.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(40.0, 0.0));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32);

    Entity truck = registry->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(0.0, 60), glm::vec2(1.0, 1.0), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(20.0, 10.0));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32);
}

void Game::Setup() {
    LoadLevel(1);
}

void Game::Update() {
    int getticks = SDL_GetTicks();

    int timeToWait = MILLISECS_PER_FRAME - (getticks - millisecsPreviousFrame);
    if (timeToWait > 0) {
        SDL_Delay(timeToWait);
    }

    // Difference in ticks since the last frame, converted to seconds
    double deltaTime = (getticks - millisecsPreviousFrame) / 1000.0;

    if (deltaTime > 0.1) {
        deltaTime = 0.1;
    }

    millisecsPreviousFrame = getticks;

    // Update from systems
    registry->GetSystem<MovementSystem>().Update(deltaTime);

    // Update the registry to process the entities that are waiting to be created/deleted
    registry->Update();
}

void Game::Render() {
    // Gray color
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255); 
    SDL_RenderClear(renderer);

    // Updating all the rendering objects
    registry->GetSystem<RenderSystem>().Update(renderer, assetStore);

    // Presents the renderer (swap the buffers to display the current frame)
    SDL_RenderPresent(renderer);
}