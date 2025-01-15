#include <iostream>
#include <SDL2/SDL.h>
#include "Game.h"

Game::Game() {
    isRunning = false;
    std::cout << "Game Constructor called!" << std::endl;
}

Game::~Game() {
    std::cout << "Game Destructor called!" << std::endl;
}

void Game::Initialize() {

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "Error initializing SDL." << std::endl;
        return;
    }

    windowWidth = 800; 
    windowHeight = 600; 

    // Creating window
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_BORDERLESS
    );

    if (!window) {
        std::cerr << "Error creating SDL Window." << std::endl;
        return;
    }
    
    // Creating Renderer
    renderer = SDL_CreateRenderer(
        window, 
        -1,   
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {
        std::cerr << "Error creating SDL Renderer window." << std::endl;
        return;
    }
    
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

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

    std::cout << "Destroying the game instance!" << std::endl;
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

}

void Game::Update() {
    // todo: update game objects
}

void Game::Render() {

    // Gray color
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255); 
    SDL_RenderClear(renderer);

    // Testing a player rectangle
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect player  = {10, 10, 20, 20};
    SDL_RenderFillRect(renderer, &player);

    SDL_RenderPresent(renderer);

}