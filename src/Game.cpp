#include <iostream>
#include <SDL2/SDL.h>
#include "Game.h"

Game::Game() {
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

    // Creating window
    SDL_Window* window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_BORDERLESS)
        ;

    if (!window) {
        std::cerr << "Error creating SDL Window." << std::endl;
        return;
    }
    
    // Creating Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer) {
        std::cerr << "Error creating SDL Renderer window." << std::endl;
        return;
    }

}

void Game::Run() {

}

void Game::Destroy() {

}

void Game::ProcessInput() {

}

void Game::Update() {

}

void Game::Render() {

}
