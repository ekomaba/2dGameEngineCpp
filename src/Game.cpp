#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
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

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    windowWidth = displayMode.w;
    windowHeight = displayMode.h;

    //windowWidth = 800; 
    //windowHeight = 600; 

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
        SDL_RENDERER_ACCELERATED);

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

// testing player moving
glm::vec2 playerPosition;
glm::vec2 playerVelocity;

void Game::Setup() {
    playerPosition = glm::vec2(10.0, 20.0);
    playerVelocity = glm::vec2(1.0, 0.0);
}

void Game::Update() {
    playerPosition.x += playerVelocity.x;
    playerPosition.y += playerVelocity.y;
}

void Game::Render() {

    // Gray color
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255); 
    SDL_RenderClear(renderer);

    // Loads a PNG texture
    SDL_Surface *surface = IMG_Load("./assets/images/tank-tiger-right.png");
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    // Free the surface after creating the texture
    // The surface is only needed as an intermediary to load the image
    // once the texture is created, the surface is no longer required.
    SDL_FreeSurface(surface);

    // Destination rectangle that will receive the texture
    SDL_Rect dstRect = { 
        static_cast<int>(playerPosition.x), 
        static_cast<int>(playerPosition.y), 
        32, 
        32};

    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_DestroyTexture(texture);

    // Presents the renderer (swap the buffers to display the current frame)
    SDL_RenderPresent(renderer);

}