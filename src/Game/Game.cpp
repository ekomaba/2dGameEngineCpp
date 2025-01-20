#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"

Game::Game() {
    isRunning = false;
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
    
    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

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

void Game::Setup() {
    // TODO: example...
    // Entity tank = registry.CreateEntity();
    // tank.AddComponent<TransformComponent>();
    // tank.AddComponent<BoxCollider>();
    // tank.AddComponent<SpriteComponent>("path...");
    
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

    // TODO:
    // MovementSystem.Update();
    // AllTheSystems...Update();
}

void Game::Render() {
    // Gray color
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255); 
    SDL_RenderClear(renderer);

    // TODO: Render game objects... 

    // Presents the renderer (swap the buffers to display the current frame)
    SDL_RenderPresent(renderer);
}