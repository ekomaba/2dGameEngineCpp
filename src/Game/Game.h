#ifndef GAME_H
#define GAME_H

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include <SDL2/SDL.h>

const int FPS = 30;
const int MILLISECS_PER_FRAME = 1000/FPS;

class Game { 
private:
    bool isRunning;
    bool isDebug;
    int millisecsPreviousFrame = 0;
    SDL_Window* window;
    SDL_Renderer* renderer;

    std::unique_ptr<Registry> registry;
    std::unique_ptr<AssetStore> assetStore;

public:
    Game();
    ~Game();
    void Initialize();
    void Run();
    void Destroy();
    void ProcessInput();
    void LoadLevel(int level);
    void Setup();
    void Update();
    void Render();

    int windowWidth;
    int windowHeight;    
};

#endif